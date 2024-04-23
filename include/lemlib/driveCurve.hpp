namespace lemlib {

/**
 * @brief abstract DriveCurve class
 *
 * This is an abstract class to enable users to provide their own, custom drive
 * curves for LemLib to use.
 */
class DriveCurve {
    public:
        /**
         * @brief Processes an input and returns an output
         *
         * This is a pure virtual function that needs to be overriden by child classes
         *
         * @param input the input to process
         * @return float output
         */
        virtual float curve(float input) = 0;
};

/**
 * @brief  ExpoDriveCurve class. Inherits from the DriveCurve class. This is a exponential drive curve.
 *
 * see https://www.desmos.com/calculator/umicbymbnl for an interactive graph
 * see https://www.vexforum.com/t/expo-drive-lemlibs-implementation for a detailed explanation
 */
class ExpoDriveCurve : public DriveCurve {
    public:
        /**
         * @brief Construct a new Expo Drive Curve object
         *
         * see https://www.desmos.com/calculator/umicbymbnl for an interactive graph
         * see https://www.vexforum.com/t/expo-drive-lemlibs-implementation for a detailed explanation
         *
         * @param deadband range where input is considered to be input
         * @param minOutput the minimum output that can be returned
         * @param curve how "curved" the graph is
         *
         * @b Example
         * @code {.cpp}
         * // create a new exponential drive curve
         * // controller deadband is set to 5
         * // minimum output is set to 12
         * // curve gain is set to 1.132
         * lemlib::ExpoDriveCurve driveCurve(5, 12, 1.132);
         * @endcode
         */
        ExpoDriveCurve(float deadband, float minOutput, float curve);
        /**
         * @brief curve an input
         *
         * @param input the input to curve
         * @return float the curved output
         *
         * @b Example
         * @code {.cpp}
         * void opcontrol() {
         *     // create a new exponential drive curve
         *     lemlib::ExpoDriveCurve driveCurve(5, 12, 1.132);
         *     // input 4
         *     std::cout << driveCurve.curve(4) << std::endl; // outputs 0 because input is within deadzone
         *     //  input 6
         *     std::cout << driveCurve.curve(6) << std::endl; // outputs 12, the minimum output
         *     std::cout << driveCurve.curve(127) << std::endl; // outputs 127, the maximum output
         * }
         * @endcode
         */
        float curve(float input);
    private:
        const float deadband = 0;
        const float minOutput = 0;
        const float curveGain = 1;
};
} // namespace lemlib