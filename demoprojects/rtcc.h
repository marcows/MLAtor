// just for building successfully, not working functionally

#define RTCCInit()
#define RTCCProcessEvents()
#define mRTCCOff()
#define mRTCCSet()
#define mRtccWrEnable()
#define mRtccWrDisable()

#define RTCCSetBinSec(Sec)
#define RTCCSetBinMin(Min)
#define RTCCSetBinHour(Hour)
#define RTCCSetBinDay(Day)
#define RTCCSetBinMonth(Month)
#define RTCCSetBinYear(Year)

#define mRTCCGetBinSec() 0
#define mRTCCGetBinMin() 0
#define mRTCCGetBinHour() 0
#define mRTCCGetBinWkDay() 0
#define mRTCCGetBinMonth() 1

#define RTCCCalculateWeekDay()

extern char _time_str[16];
extern char _date_str[16];
