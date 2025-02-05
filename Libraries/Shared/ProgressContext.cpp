#include "ProgressContext.h"

ProgressContext::ProgressContext(int total) :
    m_start(std::chrono::high_resolution_clock::now()),
    m_total(total),
    m_lastPercent(-1),
    m_lastBufferLength(0),
    m_label(""),
    m_constantTimeSeconds(0.0f)
{
}

void ProgressContext::ConstantTimeEnd()
{
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_start);

    m_constantTimeSeconds = (float)timeSpan.count();
    m_start = now;
}

void ProgressContext::SetLabel(const char* label)
{
    m_label = label;
}

std::string ProgressContext::MakeDurationString(float durationInSeconds)
{
    std::string ret;

    static const float c_oneMinute = 60.0f;
    static const float c_oneHour = c_oneMinute * 60.0f;

    int hours = int(durationInSeconds / c_oneHour);
    durationInSeconds -= float(hours) * c_oneHour;

    int minutes = int(durationInSeconds / c_oneMinute);
    durationInSeconds -= float(minutes) * c_oneMinute;

    int seconds = int(durationInSeconds);

    char buffer[1024];
    if (hours < 10)
        sprintf_s(buffer, "0%i:", hours);
    else
        sprintf_s(buffer, "%i:", hours);
    ret = buffer;

    if (minutes < 10)
        sprintf_s(buffer, "0%i:", minutes);
    else
        sprintf_s(buffer, "%i:", minutes);
    ret += buffer;

    if (seconds < 10)
        sprintf_s(buffer, "0%i", seconds);
    else
        sprintf_s(buffer, "%i", seconds);
    ret += buffer;

    return ret;
}

void ProgressContext::Show(int count)
{
    // only update if the percent has changed
    int percent = int(100.0f * float(count) / float(m_total));
    if (m_lastPercent == percent)
        return;
    m_lastPercent = percent;

    // get time since start
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_start);

    std::string elapsed = MakeDurationString(m_constantTimeSeconds + (float)timeSpan.count());

    float estimateMultiplier = std::max(float(m_total) / float(count), 1.0f);
    std::string estimate = MakeDurationString(m_constantTimeSeconds + estimateMultiplier * (float)timeSpan.count());

    // make the message
    char buffer[1024];
    if (count < 0)
    {
        sprintf_s(buffer, "\r%s100%%  elapsed %s", m_label, elapsed.c_str());
    }
    else
    {
        sprintf_s(buffer, "\r%s%i%%  elapsed %s  estimated %s", m_label, percent, elapsed.c_str(), estimate.c_str());
    }
    int newBufferLength = (int)strlen(buffer);
    int newBufferLengthCopy = newBufferLength;

    // pad with spaces to erase whatever may be there from before
    while (newBufferLength < m_lastBufferLength)
    {
        buffer[newBufferLength] = ' ';
        newBufferLength++;
    }
    buffer[newBufferLength] = 0;
    if (count < 0)
        strcat_s(buffer, "\n");

    // show the message
    printf("%s", buffer);
    m_lastBufferLength = newBufferLengthCopy;
}