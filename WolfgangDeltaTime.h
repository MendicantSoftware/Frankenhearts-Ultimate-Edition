#include <chrono>
#include <thread>

#pragma once

namespace WG {

    /**
    *Handles, and enforces delta time. The FrameEnforcer also keeps track of diagetic time.
    *
    *The FrameEnforcer only works as intended if:
    * - CreateFrameData is called at the beginning of each frame.
    * - EnforceFrameRate is called at the end of each frame.
    */
    class FrameEnforcer {
        public:

        int FPS, iFrame = 0, iSecond = 0, iMinute = 0, iHour = 0;

        std::chrono::milliseconds chronoFrameTime;
        std::chrono::high_resolution_clock::time_point chronoFrameBeginTime;



        FrameEnforcer(int TargetFrameRate) {

            FPS = TargetFrameRate;

            chronoFrameTime = std::chrono::milliseconds(1000 / FPS);
        }

        /**
        *Safe method of changing the target frame rate.
        */
        void ChangeFrameRate(int TargetFrameRate) {
            FPS = TargetFrameRate;

            chronoFrameTime = std::chrono::milliseconds(1000 / FPS);
        }

        /**
        *Must be called at the beginning of each frame in order for enforcement to work.
        *
        *Also manages diagetic time, and frame count.
        */
        int CreateFrameData() {
            chronoFrameBeginTime = std::chrono::high_resolution_clock::now();

            //Handle non-diagetic time
            if (iFrame == FPS) {
                iFrame = 0;
                iSecond++;
            } else {
                iFrame++;
            }

            if (iSecond == 60) {
                iSecond = 0;
                iMinute++;
            } 

            if (iMinute == 60) {
                iMinute = 0;
                iHour++;
            } 

            return 0;

        }

        /**
        *Enforces the target frame rate.
        */
        int EnforceFrameRate() {
            auto chronoElapasedTime = std::chrono::high_resolution_clock::now();
            auto chronoFrameDuration = chronoElapasedTime - chronoFrameBeginTime;
    

            if (chronoFrameDuration < chronoFrameTime) {
                auto chronoSleepDuration = chronoFrameTime - std::chrono::duration_cast<std::chrono::milliseconds>(chronoFrameDuration);
                std::this_thread::sleep_for(chronoSleepDuration);
            }

            return 0;

        }



    };

}