namespace lemlib {

class DriveCurve {
    public:
        virtual float curve(float input) = 0;
};

class ExpoDriveCurve : public DriveCurve {
    public:
        /**
         * @brief Construct a new Expo Drive Curve object
         *
         * see https://www.desmos.com/calculator/umicbymbnl for interactive graph
         *
         * @param deadband range where input is considered to be input
         * @param minOutput the minimum output that can be returned
         * @param curve how "curved" the graph is
         */
        ExpoDriveCurve(float deadband, float minOutput, float curve);
        /**
         * @brief curve an input
         *
         * @param input the input to curve
         * @return float the curved output
         */
        float curve(float input);
    private:
        const float deadband = 0;
        const float minOutput = 0;
        const float curveGain = 1;
};
} // namespace lemlib