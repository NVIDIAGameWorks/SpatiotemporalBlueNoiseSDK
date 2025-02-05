#pragma once

#include <chrono>
#include <string>

// A helper for reporting progress over time
class ProgressContext
{
public:
    ProgressContext(int total);

    void ConstantTimeEnd();

    void SetLabel(const char* label);

    std::string MakeDurationString(float durationInSeconds);

    void Show(int count);

private:

    std::chrono::high_resolution_clock::time_point m_start;
    int m_total;
    int m_lastPercent;
    int m_lastBufferLength;
    const char* m_label;
    float m_constantTimeSeconds;
};