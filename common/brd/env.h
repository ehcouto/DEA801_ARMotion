#ifndef ENV_H
#define ENV_H


/* #define MKV42F16 */
/* #define RX66T_SONIC */
/* #define QT_CPP_SIMULATOR */
/* #define VISUAL_STUDIO */
/* #define RX24T */
/* #define RX13T */
/* #define RX66T_STORM */
/* #define RX66T_AIRCON */
/* #define GD32F303 */
#define MCXA166_DW

#define MC_ENABLE_FIRST_MOTOR
#if defined(RX24T) | defined(RX66T_STORM) | defined(RX66T_AIRCON) | defined(MCXA166_DW)
#define MC_ENABLE_SECOND_MOTOR
#endif

#endif
