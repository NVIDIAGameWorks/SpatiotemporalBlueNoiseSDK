/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#pragma once

#include <algorithm>

#include "SimulatedAnnealing/SAData.h"
#include "SimulatedAnnealing/SADataController.h"
#include "SimulatedAnnealing/SAProgressData.h"
#include "Utils/Dimensions3D.h"

#include "STBNMath.h"
#include "STBNRandom.h"

#include "pcg_basic.h"

#include "SAProgressData.h"

template<typename T, float(*ValueDistance)(T, T)>
class SimulatedAnnealing
{
public:
	SimulatedAnnealing(const Dimensions3D& dims, SADataController<T, ValueDistance>& controller, float swapCountFactor, float coolingFactor) :
        m_dims(dims),
        m_numPixels(m_dims.x * m_dims.y * m_dims.z),
        m_controller(controller),
        m_coolingRate(1.0f / (coolingFactor * float(m_dims.NumPixels()) * float(m_dims.NumPixels()))),
        m_numSwaps(std::max(size_t(swapCountFactor * float(m_dims.NumPixels())* float(m_dims.NumPixels())), size_t(1))),
        m_pd({0.0f, 0, m_numSwaps, 0.0f})
    {
        
    }

    SAProgressData& GetProgressData()
    {
        return m_pd;
    }

    float GetCoolingRate() const
    {
        return m_coolingRate;
    }

    Dimensions3D GetDimensions() const
    {
        return m_dims;
    }

    size_t GetNumPixels() const
    {
        return m_numPixels;
    }

    size_t GetNumSwaps() const
    {
        return m_numSwaps;
    }

    void Run(pcg32_random_t& rng)
    {
        PerformInitialSplats();
        InitializeIterationState();
        while (m_pd.swapIndex < m_numSwaps)
        {
            PerformSwapIteration(rng);
        }
    }

private:
    const Dimensions3D m_dims;
    const size_t m_numPixels;
    SADataController<T, ValueDistance>& m_controller;

    float m_coolingRate;
    size_t m_numSwaps;

    SAProgressData m_pd;

    void PerformInitialSplats()
    {
        for (size_t index = 0; index < GetNumPixels(); ++index)
        {
            m_controller.SplatOn(index);
        }
    }

    void InitializeIterationState()
    {
        m_pd.energy = m_controller.Energy();
        m_pd.temperature = 1.0f;
        m_pd.swapIndex = 0;
    }

    void PerformSwapIteration(pcg32_random_t& rng)
    {
        m_pd.temperature = std::max(m_pd.temperature - m_coolingRate, 0.0f);

        uint32_t indexA = pcg32_boundedrand_r(&rng, (uint32_t)GetNumPixels());
        uint32_t indexB = pcg32_boundedrand_r(&rng, (uint32_t)GetNumPixels());
        m_controller.Swap(indexA, indexB);

        float swapEnergy = m_controller.Energy();
        if (swapEnergy <= m_pd.energy || RandomFloat01(rng) < m_pd.temperature)
        {
            m_pd.energy = swapEnergy;
        }
        else
        {
            m_controller.Swap(indexA, indexB);
        }

        m_pd.swapIndex++;
    }
};
