#include <SensorService.h>

#include <Wire.h>
#include "heartRate.h"

SensorService::SensorService() : rateSpot(0), lastBeat(0), beat(0), isFirstGetBeat(true)
{
    for (int i = 0; i < ARRAY_RATE_SIZE; ++i)
    {
        rates[i] = 0;
    }
}

bool SensorService::Start()
{
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
    {
        return false;
    }

    particleSensor.setup();
    particleSensor.setPulseAmplitudeRed(0x0A);
    particleSensor.setPulseAmplitudeGreen(0);
    return true;
}

bool SensorService::IsHeartBeatDetected()
{
    return checkForBeat(particleSensor.getIR());
}

int SensorService::GetHeartBeat()
{
    if (isFirstGetBeat)
    {
        while (rates[ARRAY_RATE_SIZE - 1] == 0)
        {
            if (checkForBeat(particleSensor.getIR()))
            {
                long delta = millis() - lastBeat;
                lastBeat = millis();

                long beatsPerMinute = 60 / (delta / 1000.0);

                if (beatsPerMinute > 20 && beatsPerMinute < 255)
                {
                    rates[rateSpot++] = (byte)beatsPerMinute;
                    rateSpot %= ARRAY_RATE_SIZE;
                }
            }
        }
        isFirstGetBeat = false;
    }
    else if (checkForBeat(particleSensor.getIR()))
    {
        long delta = millis() - lastBeat;
        lastBeat = millis();

        long beatsPerMinute = 60 / (delta / 1000.0);

        if (beatsPerMinute > 20 && beatsPerMinute < 255)
        {
            rates[rateSpot++] = (byte)beatsPerMinute;
            rateSpot %= ARRAY_RATE_SIZE;

            beat = 0;
            for (int i = 0; i < ARRAY_RATE_SIZE; ++i)
            {
                beat += rates[i];
            }

            beat /= ARRAY_RATE_SIZE;
        }
    }

    return beat;
}

void SensorService::Clear()
{
    rateSpot = 0;
    lastBeat = 0;
    beat = 0;
    isFirstGetBeat = true;

    for (int i = 0; i < ARRAY_RATE_SIZE; ++i)
    {
        rates[i] = 0;
    }
}
