#ifndef PBUTILITY_H
#define PBUTILITY_H

#include <cstdint>

namespace future {

    template<typename T, typename P>
    union Converter {
        static_assert(sizeof(T) == sizeof(P), "size not match");
        T first;
        P second;
    };

    static inline int64_t Float64ToInt64(double v) {
        Converter<double, int64_t> converter;
        converter.first = v;
        return converter.second;
    }

    static inline int32_t Float32ToInt32(float v) {
        Converter<float, int32_t> converter;
        converter.first = v;
        return converter.second;
    }

    static inline double Int64ToFloat64(int64_t v) {
        Converter<double, int64_t> converter;
        converter.second = v;
        return converter.first;
    }

    static inline float Int32ToFloat32(int32_t v) {
        Converter<float, int32_t> converter;
        converter.second = v;
        return converter.first;
    }

    static inline uint64_t Int64ToUInt64(int64_t v) {
        Converter<int64_t, uint64_t> converter;
        converter.first = v;
        return converter.second;
    }

    static inline int64_t UInt64ToInt64(uint64_t v) {
        Converter<int64_t, uint64_t> converter;
        converter.second = v;
        return converter.first;
    }

    static inline uint32_t Int32ToUInt32(int32_t v) {
        Converter<int32_t, uint32_t> converter;
        converter.first = v;
        return converter.second;
    }

    static inline int32_t UInt32ToInt32(uint32_t v) {
        Converter<int32_t, uint32_t> converter;
        converter.second = v;
        return converter.first;
    }

    static inline int32_t LogicalRightShift32(int32_t value, int32_t spaces) {
        return UInt32ToInt32((Int32ToUInt32(value) >> spaces));
    }

    static inline int64_t LogicalRightShift64(int64_t value, int32_t spaces) {
        return UInt64ToInt64((Int64ToUInt64(value) >> spaces));
    }

    constexpr uint32_t littleEdian32Size = 4;

    constexpr uint32_t PbFloatSize(float value) {
        return littleEdian32Size;
    }

    constexpr uint32_t PbFixed32Size(int32_t value) {
        return littleEdian32Size;
    }

    constexpr uint32_t littleEdian64Size = 8;

    constexpr uint32_t PbDoubleSize(double value) {
        return littleEdian64Size;
    }

    constexpr uint32_t PbBoolSize(bool value) {
        return 1;
    }

    extern uint32_t PbRawVarint32Size(int32_t value);

    extern uint32_t PbInt64Size(int64_t value);

    static inline uint32_t PbInt32Size(int32_t value) {
        if (value >= 0) {
            return PbRawVarint32Size(value);
        } else {
            return 10;
        }
    }
}
#endif //PBUTILITY_H
