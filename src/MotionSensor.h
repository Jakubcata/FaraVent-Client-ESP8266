#ifndef __MOTION_H__
#define __MOTION_H__

/* Motion sensor DIO pin nubmer */
#define MOTION_SENSOR_PIN       15

class MotionSensor
{
    uint8_t iPin;

    public:
        MotionSensor( uint8_t aPin ): iPin( aPin )
        {            
        }

        void Init()
        {
            pinMode( iPin, INPUT );
        }

        bool IsMovement() const
        {
            return digitalRead( iPin );
        }
};

#endif /* __MOTION_H__ */