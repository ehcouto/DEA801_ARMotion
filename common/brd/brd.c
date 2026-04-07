/**
*
*
*/

#include "brd.h"
#ifdef QT_CPP_SIMULATOR
#include "brd_qt.h"
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
static brd_t brd_M1;
#endif
#ifdef MC_ENABLE_SECOND_MOTOR
static brd_t brd_M2;
#endif

#ifdef MANUAL_PWM_TEST
float debug_dutyU,debug_dutyV,debug_dutyW;
#endif
/**
* @brief Board layer initialization function
* 
* Function will load all initial values of parameters and variables about board layer. All related 
* sublayers or process related to board layer will be initialized in this function body.* 
*/
void brdInit(void (*_McFuncFastLoop)(int32_t MxIndex), void (*_McFuncSlowLoop)(int32_t MxIndex),
             void (*_AppFuncFastLoop)(void), void (*_AppFuncSlowLoop)(void),
             void (*_AppCommunicationHandler)(void))
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
    drvInitMcu(_McFuncFastLoop, _McFuncSlowLoop, _AppFuncFastLoop, _AppFuncSlowLoop, _AppCommunicationHandler);
#elif defined(GD32F303)
    drvInitMcu(_McFuncFastLoop, _McFuncSlowLoop, _AppFuncFastLoop, _AppFuncSlowLoop, _AppCommunicationHandler);
#elif defined(QT_CPP_SIMULATOR)
    qtInitMcu(_McFuncFastLoop, _McFuncSlowLoop);
    qt_brdInit = true;
#elif defined(VISUAL_STUDIO)
    vsbrdInit(_McFuncFastLoop, _McFuncSlowLoop);
#elif defined(RX24T)
    drvInitMcu(_McFuncFastLoop, _McFuncSlowLoop, _AppFuncFastLoop, _AppFuncSlowLoop);
#elif defined(RX13T)
    drvInitMcu(_McFuncFastLoop, _McFuncSlowLoop,_AppFuncFastLoop, _AppFuncSlowLoop, _AppCommunicationHandler);
#elif defined(RX66T_STORM)
    drvInitMcu(_McFuncFastLoop, _McFuncSlowLoop, _AppFuncFastLoop, _AppFuncSlowLoop, _AppCommunicationHandler);
#else
#error Process environment should be defined;
#endif
}

/**
* @brief Enable Phase U related PWM output
* 
* Function enables PWM module related output for Phase U
*/
#ifdef MC_ENABLE_FIRST_MOTOR
void brdPwmEnablePhaseU_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303)
    drvPwmEnablePhaseU();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnablePhaseU = true;
#elif defined(VISUAL_STUDIO)
    printf("brdPwmEnablePhaseU_M1\n");
    vsbrdPwmEnablePhaseU = true;
#elif defined(RX24T) || defined(MCXA166_DW)

#elif defined(RX66T_STORM)
    drvPwmEnablePhaseU();
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdPwmEnablePhaseU_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303)
	//drvPwmEnablePhaseU_M2();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnablePhaseU = true;
#elif defined(VISUAL_STUDIO)
    fprintf(stdout, "brdPwmEnablePhaseU_M2\n");
    vsbrdPwmEnablePhaseU = true;
#elif defined(RX24T) || defined(MCXA166_DW)

#elif defined(RX66T_STORM)
    drvPwmEnablePhaseU_M2();
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief Enable Phase V related PWM output
* 
* Function enables PWM module related output for Phase V
*/
#ifdef MC_ENABLE_FIRST_MOTOR
void brdPwmEnablePhaseV_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303)
    drvPwmEnablePhaseV();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnablePhaseV = true;
#elif defined(VISUAL_STUDIO)
    printf("brdPwmEnablePhaseV_M1\n");
    vsbrdPwmEnablePhaseV = true;
#elif defined(RX24T) || defined(MCXA166_DW)

#elif defined(RX66T_STORM)
    drvPwmEnablePhaseV();
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdPwmEnablePhaseV_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303)
	//drvPwmEnablePhaseV_M2();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnablePhaseV = true;
#elif defined(VISUAL_STUDIO)
    printf("brdPwmEnablePhaseV_M2\n");
    vsbrdPwmEnablePhaseV = true;
#elif defined(RX24T) || defined(MCXA166_DW)

#elif defined(RX66T_STORM)
	drvPwmEnablePhaseV_M2();
#else
#error Process environment should be defined;
#endif
}
#endif


/**
* @brief Enable Phase W related PWM output
* 
* Function enables PWM module related output for Phase W
*/
#ifdef MC_ENABLE_FIRST_MOTOR
void brdPwmEnablePhaseW_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303)
    drvPwmEnablePhaseW();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnablePhaseW = true;
#elif defined(VISUAL_STUDIO)
    printf("brdPwmEnablePhaseW_M1\n");
    vsbrdPwmEnablePhaseW = true;
#elif defined(RX24T) || defined(MCXA166_DW)

#elif defined(RX66T_STORM)
    drvPwmEnablePhaseW();
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdPwmEnablePhaseW_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303)

#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnablePhaseW = true;
#elif defined(VISUAL_STUDIO)
    printf("brdPwmEnablePhaseW_M2\n");
    vsbrdPwmEnablePhaseW = true;
#elif defined(RX24T) || defined(MCXA166_DW)

#elif defined(RX66T_STORM)
	drvPwmEnablePhaseW_M2();
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief Enable Phases UVW related PWM output
* 
* Function enables PWM module related output for Phase W
*/
#ifdef MC_ENABLE_FIRST_MOTOR
void brdPwmEnablePhaseUVW_M1(void)
{
#if defined(MKV42F16) || defined(RX24T)

#elif defined(RX66T_SONIC) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
	drvPwmEnablePhaseUVW();
#elif defined(RX66T_STORM)
	drvPwmEnablePhaseUVW();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnablePhaseW = true;
#elif defined(VISUAL_STUDIO)
    printf("brdPwmEnablePhaseUVW_M1\n");
    vsbrdPwmEnablePhaseUVW= true;
#elif defined(RX13T)
    drvPwmEnablePhaseUVW();
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdPwmEnablePhaseUVW_M2(void)
{
#if defined(MKV42F16) || defined(RX24T) || defined(RX13T)

#elif defined(RX66T_SONIC) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
	drvPwmEnablePhaseUVW();
#elif defined(RX66T_STORM)
	drvPwmEnablePhaseUVW_M2();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnablePhaseW = true;
#elif defined(VISUAL_STUDIO)
    printf("brdPwmEnablePhaseUVW_M2\n");
    vsbrdPwmEnablePhaseUVW = true;
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief Set PWM duty cycles
*
* Function sets PWM duties as the given function parameters. Float type parameters are desired
*
* @param[in]    dutyU   duty cycle value for phase U
* @param[in]    dutyV   duty cycle value for phase V
* @param[in]    dutyW   duty cycle value for phase W
*/
#ifdef MC_ENABLE_FIRST_MOTOR
uint32_t cnt_brdSetPwmDuties_M1,cnt_brdSetPwmDuties_M2;
void brdSetPwmDuties_M1(float dutyU, float dutyV, float dutyW)
{
	++cnt_brdSetPwmDuties_M1;
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
	#if defined(AUTO_PWM_TEST)
		drvSetPwmDuties(0.1f, 0.1f, 0.1f);
	#else
		drvSetPwmDuties(dutyU, dutyV, dutyW);
	#endif
    //drvSetPwmDuties(dutyU, dutyV, dutyW);
#elif defined(RX66T_STORM)
    drvSetPwmDuties(dutyU, dutyV, dutyW);
#elif defined(QT_CPP_SIMULATOR)
    qtDutyU = dutyU;
    qtDutyV = dutyV;
    qtDutyW = dutyW;
    qt_brdSetPwmDuties = true;
#elif defined(VISUAL_STUDIO)
    printf("brdSetPwmDuties_M1 dutyU = %f, dutyV = %f, dutyW = %f\n", dutyU, dutyV, dutyW);
    vsDutyU = dutyU;
    vsDutyV = dutyV;
    vsDutyW = dutyW;
    vsbrdSetPwmDuties = true;
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief Set PWM duty cycles
*
* Function sets PWM duties as the given function parameters. Float type parameters are desired
*
* @param[in]    dutyU   duty cycle value for phase U
* @param[in]    dutyV   duty cycle value for phase V
* @param[in]    dutyW   duty cycle value for phase W
*/
#ifdef MC_ENABLE_SECOND_MOTOR
void brdSetPwmDuties_M2(float dutyU, float dutyV, float dutyW)
{		
	++cnt_brdSetPwmDuties_M2;
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(QT_CPP_SIMULATOR)
    qtDutyU = dutyU;
    qtDutyV = dutyV;
    qtDutyW = dutyW;
    qt_brdSetPwmDuties = true;
#elif defined(VISUAL_STUDIO)
    printf("brdSetPwmDuties_M2 dutyU = %f, dutyV = %f, dutyW = %f\n", dutyU, dutyV, dutyW);
    vsDutyU = dutyU;
    vsDutyV = dutyV;
    vsDutyW = dutyW;
    vsbrdSetPwmDuties = true;
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
	#if defined(AUTO_PWM_TEST)
		drvSetPwmDuties_M2(0.7f, 0.6f, 0.4f);
	#elif defined(MANUAL_PWM_TEST)
		drvSetPwmDuties(debug_dutyU,debug_dutyV,debug_dutyW);
	#else
		drvSetPwmDuties_M2(dutyU, dutyV, dutyW);
	#endif
	
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief Turn on inrush relay
*
* Supplies bara voltage via inrush relay
*
* @return       boolean value shows success status of the function; any case returns true
*/
void brdTurnOnInrushRelay(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX66T_AIRCON) || defined(GD32F303)
    drvTurnOnInrushRelay();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdTurnOnInrushRelay = true;
#elif defined(VISUAL_STUDIO)
    printf("brdTurnOnInrushRelay\n");
    vsbrdTurnOnInrushRelay = true;
#elif defined(RX24T) || defined(RX13T)|| defined(MCXA166_DW)

#elif defined(RX66T_STORM)
    drvTurnOnInrushRelay();
#else
#error Process environment should be defined;
#endif
}

/**
* @brief Turn off inrush relay
*
* Disconnects bara voltage to main supply
*
* @return       boolean value shows success status of the function; any case returns true
*/
void brdTurnOffInrushRelay(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX66T_AIRCON) || defined(GD32F303)
    drvTurnOffInrushRelay();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdTurnOffInrushRelay = true;
#elif defined(VISUAL_STUDIO)
    printf("brdTurnOffInrushRelay\n");
    vsbrdTurnOffInrushRelay = true;
#elif defined(RX24T) || defined(RX13T)|| defined(MCXA166_DW)

#elif defined(RX66T_STORM)
    drvTurnOffInrushRelay();
#else
#error Process environment should be defined;
#endif
}

/**
* @brief map adc channels
* 
* Function is an interface for driver side to map correct adc channels for the first parallel sampling 
*
* @return       float type phase U current
*/
#ifdef MC_ENABLE_FIRST_MOTOR
void brdMapAdcChannels_M1(uint8_t sec)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303)
    drvMapAdcChannels(sec);
#elif defined(RX66T_STORM) || defined(MCXA166_DW)
    //drvMapAdcChannels(sec);
#elif defined(QT_CPP_SIMULATOR)
    qt_brdMapAdcChannels = true;
#elif defined(VISUAL_STUDIO)
    printf("brdMapAdcChannels_M1\n");
    vsbrdMapAdcChannels = true;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdMapAdcChannels_M2(uint8_t sec)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T) || defined(MCXA166_DW)

#elif defined(RX66T_STORM)
    //drvMapAdcChannels_M2(sec);
#elif defined(QT_CPP_SIMULATOR)
    qt_brdMapAdcChannels = true;
#elif defined(VISUAL_STUDIO)
    printf(stdout, "brdMapAdcChannels_M2\n");
    vsbrdMapAdcChannels = true;
#elif defined(RX24T) || defined(RX66T_AIRCON)
	drvMapAdcChannels_M2(sec);
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief sample adc channels
* 
* Function samples adc channels according to current sector 
*
* @return       no return value
*/
#ifdef MC_ENABLE_FIRST_MOTOR
void brdSampleCurrentsUVW_M1(uint8_t sec)
{
#if defined(MKV42F16)
    drvSamplePhaseCurrents();
    switch(sec)
    {
      case 2 :
      case 3 :
        {
            brd_M1.sampledCurrentW = -currentWa;
            brd_M1.sampledCurrentU = -currentUb;
            brd_M1.sampledCurrentV = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentU);
        }
        break;
      case 4 :
      case 5 :
        {
            brd_M1.sampledCurrentV = -currentVa;
            brd_M1.sampledCurrentU = -currentUb;
            brd_M1.sampledCurrentW = -(brd_M1.sampledCurrentU + brd_M1.sampledCurrentV);
        }
        break;
      case 6 :
      case 1 :
        {
            brd_M1.sampledCurrentW = -currentWa;
            brd_M1.sampledCurrentV = -currentVb;
            brd_M1.sampledCurrentU = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentV);
        }
        break;
      default :
        {
          ;
        }
    }
#elif defined(GD32F303)
    drvSampleAdcChannels(sec);
    drvSamplePhaseCurrents();
    switch(sec)
    {
    case 2 :
    case 3 :
      {
        brd_M1.sampledCurrentW = -currentW1;
        brd_M1.sampledCurrentU = -currentU0;
        brd_M1.sampledCurrentV = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentU);
      }
      break;
    case 4 :
    case 5 :
      {
        brd_M1.sampledCurrentV = -currentV1;
        brd_M1.sampledCurrentU = -currentU0;
        brd_M1.sampledCurrentW = -(brd_M1.sampledCurrentU + brd_M1.sampledCurrentV);
      }
      break;
    case 6 :
    case 1 :
      {
        brd_M1.sampledCurrentW = -currentW1;
        brd_M1.sampledCurrentV = -currentV0;
        brd_M1.sampledCurrentU = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentV);
      }
      break;
    default :
      {
        ;
      }
    }
#elif defined(RX66T_SONIC)
    drvSampleAdcChannels(sec);
    drvSamplePhaseCurrents();
    switch(sec)
    {
      case 2 :
      case 3 :
        {
            brd_M1.sampledCurrentW = -currentW1;
            brd_M1.sampledCurrentU = -currentU0;
            brd_M1.sampledCurrentV = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentU);
        }
        break;
      case 4 :
      case 5 :
        {
            brd_M1.sampledCurrentV = -currentV1;
            brd_M1.sampledCurrentU = -currentU0;
            brd_M1.sampledCurrentW = -(brd_M1.sampledCurrentU + brd_M1.sampledCurrentV);
        }
        break;
      case 6 :
      case 1 :
        {
            brd_M1.sampledCurrentW = -currentW1;
            brd_M1.sampledCurrentV = -currentV0;
            brd_M1.sampledCurrentU = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentV);
        }
        break;
      default :
        {
            ;
        }
    }
#elif defined(RX66T_STORM)
    drvSampleAdcChannels(sec);
    drvSamplePhaseCurrents();

    brd_M1.sampledCurrentW = -currentW1;
    brd_M1.sampledCurrentV = -currentV0;
    brd_M1.sampledCurrentU = -currentU0;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdSampleCurrentsUVW = true;
#elif defined(VISUAL_STUDIO)
    printf("brdSampleCurrentsUVW_M1\n");
    vsbrdSampleCurrentsUVW = true;
#elif defined(RX24T) || defined(RX66T_AIRCON)
    drvSampleAdcChannels(sec);
    drvSamplePhaseCurrents();

    brd_M1.sampledCurrentW = currentWa;
    brd_M1.sampledCurrentU = currentUb;
    brd_M1.sampledCurrentV = (brd_M1.sampledCurrentW + brd_M1.sampledCurrentU);

#elif defined(MCXA166_DW)

    drvSampleAdcChannels(sec);
    drvSamplePhaseCurrents();
    /*
    brd_M1.sampledCurrentW = -currentWa;
    brd_M1.sampledCurrentU = -currentUb;
    brd_M1.sampledCurrentV = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentU);
    */
    switch(sec)
    {
      case 2 :
      case 3 :
        {
            brd_M1.sampledCurrentV = -currentVa;
            brd_M1.sampledCurrentU = -currentUb;
            brd_M1.sampledCurrentW = -(brd_M1.sampledCurrentU + brd_M1.sampledCurrentV);

        }
        break;
      case 4 :
      case 5 :
        {
            brd_M1.sampledCurrentW = -currentWa;
            brd_M1.sampledCurrentV = -currentVa;
            brd_M1.sampledCurrentU = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentV);
        }
        break;
      case 6 :
      case 1 :
        {
            brd_M1.sampledCurrentW = -currentWa;
            brd_M1.sampledCurrentU = -currentUb;
            brd_M1.sampledCurrentV = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentU);
        }
        break;
      default :
        {
            brd_M1.sampledCurrentW = -currentWa;
            brd_M1.sampledCurrentU = -currentUb;
            brd_M1.sampledCurrentV = -currentVa;
        }
    }

#elif defined(RX13T)
    drvSampleAdcChannels(sec);
    drvSamplePhaseCurrents(sec);
    switch(sec)
    {
      case 2 :
      case 3 :
        {
        	brd_M1.sampledCurrentW = -currentW1;
        	brd_M1.sampledCurrentU = -currentU0;
        	brd_M1.sampledCurrentV = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentU);
        }
        break;
      case 4 :
      case 5 :
        {
        	brd_M1.sampledCurrentV = -currentV0;
        	brd_M1.sampledCurrentU = -currentU0;
        	brd_M1.sampledCurrentW = -(brd_M1.sampledCurrentU + brd_M1.sampledCurrentV);
        }
        break;
      case 6 :
      case 1 :
        {
        	brd_M1.sampledCurrentW = -currentW1;
        	brd_M1.sampledCurrentV = -currentV0;
        	brd_M1.sampledCurrentU = -(brd_M1.sampledCurrentW + brd_M1.sampledCurrentV);
        }
        break;
      default :
        {
            ;
        }
    }
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdSampleCurrentsUVW_M2(uint8_t sec)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)
#elif defined(QT_CPP_SIMULATOR)
    qt_brdSampleCurrentsUVW = true;
#elif defined(VISUAL_STUDIO)
    printf("brdSampleCurrentsUVW_M2\n");
    vsbrdSampleCurrentsUVW = true;
#elif defined(RX66T_STORM)
       drvSampleAdcChannels_M2(sec);
       drvSamplePhaseCurrents_M2();

       brd_M2.sampledCurrentW = -currentW1_M2;
       brd_M2.sampledCurrentV = -currentV0_M2;
       brd_M2.sampledCurrentU = -currentU0_M2;

       /*
       switch(sec)
       {
         case 2 :
         case 3 :
           {
               brd_M2.sampledCurrentW = -currentW1_M2;
               brd_M2.sampledCurrentU = -currentU0_M2;
               brd_M2.sampledCurrentV = -(brd_M2.sampledCurrentW + brd_M2.sampledCurrentU);
           }
           break;
         case 4 :
         case 5 :
           {
               brd_M2.sampledCurrentV = -currentV1_M2;
               brd_M2.sampledCurrentU = -currentU0_M2;
               brd_M2.sampledCurrentW = -(brd_M2.sampledCurrentU + brd_M2.sampledCurrentV);
           }
           break;
         case 6 :
         case 1 :
           {
               brd_M2.sampledCurrentW = -currentW1_M2;
               brd_M2.sampledCurrentV = -currentV0_M2;
               brd_M2.sampledCurrentU = -(brd_M2.sampledCurrentW + brd_M2.sampledCurrentV);
           }
           break;
         default :
           {
               ;
           }
       }
       */
#elif defined(RX24T) || defined(RX66T_AIRCON)
    drvSampleAdcChannels_M2(sec);
    drvSamplePhaseCurrents_M2();
   
    brd_M2.sampledCurrentW = -currentWa_M2;
    brd_M2.sampledCurrentU = -currentUb_M2;
    brd_M2.sampledCurrentV = -(brd_M2.sampledCurrentW + brd_M2.sampledCurrentU);

#elif defined(MCXA166_DW)
    drvSampleAdcChannels_M2(sec);
    drvSamplePhaseCurrents_M2();

    switch(sec)
    {
      case 2 :
      case 3 :
        {
            brd_M2.sampledCurrentV = -currentVa_M2;
            brd_M2.sampledCurrentU = -currentUb_M2;
            brd_M2.sampledCurrentW = -(brd_M2.sampledCurrentU + brd_M2.sampledCurrentV);

        }
        break;
      case 4 :
      case 5 :
        {
            brd_M2.sampledCurrentW = -currentWa_M2;
            brd_M2.sampledCurrentV = -currentVa_M2;
            brd_M2.sampledCurrentU = -(brd_M2.sampledCurrentW + brd_M2.sampledCurrentV);
        }
        break;
      case 6 :
      case 1 :
        {
            brd_M2.sampledCurrentW = -currentWa_M2;
            brd_M2.sampledCurrentU = -currentUb_M2;
            brd_M2.sampledCurrentV = -(brd_M2.sampledCurrentW + brd_M2.sampledCurrentU);
        }
        break;
      default :
        {
            brd_M2.sampledCurrentW = -currentWa_M2;
            brd_M2.sampledCurrentU = -currentUb_M2;
            brd_M2.sampledCurrentV = -currentVa_M2;
        }
    }


#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief samples phase U current
* 
* Function enables PWM module related output for Phase U. Dividing resistors; defined
* as board parameters; are important for the resultant sampled current. 
*
* @return       float type phase U current
*/
#ifdef MC_ENABLE_FIRST_MOTOR
float brdGetPhaseCurrentU_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    return brd_M1.sampledCurrentU;
#elif defined(RX66T_STORM)
    return brd_M1.sampledCurrentU;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetPhaseCurrentU = true;
    return -qtPhaseCurrentU;
#elif defined(VISUAL_STUDIO)
    printf("brdGetPhaseCurrentU_M1 --> vsPhaseCurrentU = %f\n", vsPhaseCurrentU);
    vsbrdGetPhaseCurrentUb = true;
    return -vsPhaseCurrentU;
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief samples phase U crrent; from ADC module b
* 
* Function enables PWM module related output for Phase U. Dividing resistors; defined
* as board parameters; are important for the resultant sampled current. 
*
* @return       float type phase U current
*/
#ifdef MC_ENABLE_SECOND_MOTOR
float brdGetPhaseCurrentU_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(RX66T_STORM)
	return brd_M2.sampledCurrentU;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetPhaseCurrentUb = true;
    return -qtPhaseCurrentU;
#elif defined(VISUAL_STUDIO)
    printf("brdGetPhaseCurrentU_M2 --> vsPhaseCurrentU = %f\n", vsPhaseCurrentU);
    vsbrdGetPhaseCurrentUb = true;
    return -vsPhaseCurrentU;
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
	return brd_M2.sampledCurrentU;
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief samples phase V current
* 
* Function enables PWM module related output for Phase V. Dividing resistors; defined
* as board parameters; are important for the resultant sampled current. 
*
* @return       float type phase V current
*/
#ifdef MC_ENABLE_FIRST_MOTOR
float brdGetPhaseCurrentV_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    return brd_M1.sampledCurrentV;
#elif defined(RX66T_STORM)
    return brd_M1.sampledCurrentV;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetPhaseCurrentV = true;
    return -qtPhaseCurrentV;
#elif defined(VISUAL_STUDIO)
    printf("brdGetPhaseCurrentV_M1 --> vsPhaseCurrentV = %f\n", vsPhaseCurrentV);
    vsbrdGetPhaseCurrentVa = TRUE;
    return -vsPhaseCurrentV;
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief samples phase V current; from ADC module "a"
* 
* Function enables PWM module related output for Phase V. Dividing resistors; defined
* as board parameters; are important for the resultant sampled current. 
*
* @return       float type phase V current
*/
#ifdef MC_ENABLE_SECOND_MOTOR
float brdGetPhaseCurrentV_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(RX66T_STORM)
	return brd_M2.sampledCurrentV;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetPhaseCurrentVa = true;
    return -qtPhaseCurrentV;
#elif defined(VISUAL_STUDIO)
    printf("brdGetPhaseCurrentV_M2 --> vsPhaseCurrentV = %f\n", vsPhaseCurrentV);
    vsbrdGetPhaseCurrentVa = true;
    return -vsPhaseCurrentV;
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
	return brd_M2.sampledCurrentV;
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief samples phase W current; from ADC module "a" 
* 
* Function enables PWM module related output for Phase W. Dividing resistors; defined
* as board parameters; are important for the resultant sampled current. 
*
* @return       float type phase W current
*/
#ifdef MC_ENABLE_FIRST_MOTOR
float brdGetPhaseCurrentW_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    return brd_M1.sampledCurrentW;
#elif defined(RX66T_STORM)
    return brd_M1.sampledCurrentW;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetPhaseCurrentW = true;
    return -qtPhaseCurrentW;
#elif defined(VISUAL_STUDIO)
    printf("brdGetPhaseCurrentW_M1 --> vsPhaseCurrentW = %f\n", vsPhaseCurrentW);
    vsbrdGetPhaseCurrentWa = true;
    return -vsPhaseCurrentW;
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief samples phase W current; from ADC module "a" 
* 
* Function enables PWM module related output for Phase W. Dividing resistors; defined
* as board parameters; are important for the resultant sampled current. 
*
* @return       float type phase W current
*/
#ifdef MC_ENABLE_SECOND_MOTOR
float brdGetPhaseCurrentW_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(RX66T_STORM)
	 return brd_M2.sampledCurrentW;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetPhaseCurrentWa = true;
    return -qtPhaseCurrentW;
#elif defined(VISUAL_STUDIO)
    printf("brdGetPhaseCurrentW_M2 --> vsPhaseCurrentW = %f\n", vsPhaseCurrentW);
    vsbrdGetPhaseCurrentWa = true;
    return -vsPhaseCurrentW;
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
	return brd_M2.sampledCurrentW;
#else
#error Process environment should be defined;
#endif
}
#endif

/**
* @brief samples dc bus voltage level 
* 
* Function samples related ADC for dc bus voltage level and calculate the
* voltage as "volt" unit
* @return       float type phase W current
*/
#ifdef MC_ENABLE_FIRST_MOTOR
float brdGetDcBusVoltage_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    return drvGetDcBusVoltage();
#elif defined(RX66T_STORM)
	return drvGetDcBusVoltage();
#elif defined(QT_CPP_SIMULATOR)
    qt_brGetDcBusVoltage = true;
    return qtVbus;
#elif defined(VISUAL_STUDIO)
    printf("brGetDcBusVoltage_M1 --> vsVbus = %f\n", vsVbus);
    vsbrGetDcBusVoltage = true;
    return vsVbus;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
float brdGetDcBusVoltage_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(RX66T_STORM)
	return drvGetDcBusVoltage_M2();
#elif defined(QT_CPP_SIMULATOR)
    qt_brGetDcBusVoltage = true;
    return qtVbus;
#elif defined(VISUAL_STUDIO)
    printf("brGetDcBusVoltage_M2 --> vsVbus = %f\n", vsVbus);
    vsbrGetDcBusVoltage = true;
    return vsVbus;
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
	return drvGetDcBusVoltage_M2();
#else
#error Process environment should be defined;
#endif
}
#endif

#if defined(MKV42F16) || defined(RX24T) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
typedef struct{
    struct{
        uint16_t Wa;
        uint16_t Va;
        uint16_t Vb;
        uint16_t Ub;
    } brdAdcResult;
    struct{
        uint32_t Wa;
        uint32_t Va;
        uint32_t Vb;
        uint32_t Ub;
    } adcResultSum;
    struct{
        uint16_t Wa;
        uint16_t Va;
        uint16_t Vb;
        uint16_t Ub;
    } brdAdcOffset;
    uint16_t adcResultCntr;
    uint16_t adcResultCntrMax;
    struct{
        float Wa;
        float Va;
        float Vb;
        float Ub;
    } brdPhCurrents;
} PhCurrents_t;
#elif defined(RX66T_SONIC) || defined(GD32F303)
typedef struct{
    struct{
        uint16_t W1;
        uint16_t V1;
        uint16_t V0;
        uint16_t U0;
    } brdAdcResult;
    struct{
        uint32_t W1;
        uint32_t V1;
        uint32_t V0;
        uint32_t U0;
    } adcResultSum;
    struct{
        uint16_t W1;
        uint16_t V1;
        uint16_t V0;
        uint16_t U0;
    } brdAdcOffset;
    uint16_t adcResultCntr;
    uint16_t adcResultCntrMax;
    struct{
        float W1;
        float V1;
        float V0;
        float U0;
    } brdPhCurrents;
} PhCurrents_t;
#elif defined(RX66T_STORM)
typedef struct{
    struct{
        uint16_t W1;
        uint16_t V1;
        uint16_t V0;
        uint16_t U0;
    } brdAdcResult;
    struct{
        uint32_t W1;
        uint32_t V1;
        uint32_t V0;
        uint32_t U0;
    } adcResultSum;
    struct{
        uint16_t W1;
        uint16_t V1;
        uint16_t V0;
        uint16_t U0;
    } brdAdcOffset;
    uint16_t adcResultCntr;
    uint16_t adcResultCntrMax;
    struct{
        float W1;
        float V1;
        float V0;
        float U0;
    } brdPhCurrents;
} PhCurrents_t;
#elif defined(RX13T)
typedef struct{
    struct{
        uint16_t Wa;
        uint16_t Va;
        uint16_t Vb;
        uint16_t Ub;
    } brdAdcResult;
    struct{
        uint32_t W1;
        uint32_t V0;
        uint32_t U0;
    } adcResultSum;
    struct{
        uint16_t W1;
        uint16_t V0;
        uint16_t U0;
    } brdAdcOffset;
    uint16_t adcResultCntr;
    uint16_t adcResultCntrMax;
    struct{
    	float Wa;
    	float Va;
    	float Vb;
    	float Ub;
    } brdPhCurrents;
} PhCurrents_t;
#else
#error Process environment should be defined;
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
PhCurrents_t phCurrents_M1;
#endif
#ifdef MC_ENABLE_SECOND_MOTOR
PhCurrents_t phCurrents_M2;
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
bool brdCurrentCalibrationInit_M1(void)
{
#if defined(MKV42F16) || defined(RX24T) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
    phCurrents_M1.adcResultSum.Wa = 0;
    phCurrents_M1.adcResultSum.Va = 0;
    phCurrents_M1.adcResultSum.Vb = 0;
    phCurrents_M1.adcResultSum.Ub = 0;

    phCurrents_M1.adcResultCntr = 0;
    phCurrents_M1.adcResultCntrMax = 8000;    

    return false;
#elif defined(RX66T_SONIC) || defined(GD32F303)
    phCurrents_M1.adcResultSum.W1 = 0;
    phCurrents_M1.adcResultSum.V1 = 0;
    phCurrents_M1.adcResultSum.V0 = 0;
    phCurrents_M1.adcResultSum.U0 = 0;

    phCurrents_M1.adcResultCntr = 0;
    phCurrents_M1.adcResultCntrMax = 8000;    

    return false;
#elif defined(RX66T_STORM)
    phCurrents_M1.adcResultSum.W1 = 0;
    phCurrents_M1.adcResultSum.V1 = 0;
    phCurrents_M1.adcResultSum.V0 = 0;
    phCurrents_M1.adcResultSum.U0 = 0;

    phCurrents_M1.adcResultCntr = 0;
    phCurrents_M1.adcResultCntrMax = 8000;    

    return false;
#elif defined(RX13T)

    phCurrents_M1.adcResultSum.W1 = 0;
    phCurrents_M1.adcResultSum.V0 = 0;
    phCurrents_M1.adcResultSum.U0 = 0;

    phCurrents_M1.adcResultCntr = 0;
    phCurrents_M1.adcResultCntrMax = 8000;

    return false;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdCurrentCalibrationInit = true;
    return false;
#elif defined(VISUAL_STUDIO)
    printf("brdCurrentCalibrationInit_M1\n");
    vsbrdCurrentCalibrationInit = true;
    return false;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
bool brdCurrentCalibrationInit_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(RX66T_STORM)
    phCurrents_M2.adcResultSum.W1 = 0;
    phCurrents_M2.adcResultSum.V0 = 0;
    phCurrents_M2.adcResultSum.V1 = 0;
    phCurrents_M2.adcResultSum.U0 = 0;

    phCurrents_M2.adcResultCntr = 0;
    phCurrents_M2.adcResultCntrMax = 8000;

    return false;
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
    phCurrents_M2.adcResultSum.Wa = 0;
    phCurrents_M2.adcResultSum.Va = 0;
    phCurrents_M2.adcResultSum.Vb = 0;
    phCurrents_M2.adcResultSum.Ub = 0;

    phCurrents_M2.adcResultCntr = 0;
    phCurrents_M2.adcResultCntrMax = 8000;    

    return false;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdCurrentCalibrationInit = true;
    return false;
#elif defined(VISUAL_STUDIO)
    printf("brdCurrentCalibrationInit_M2\n");
    vsbrdCurrentCalibrationInit = true;
    return false;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
bool brdCurrentCalibrationHandler_M1(void)
{
#if defined(MKV42F16)
    if(phCurrents_M1.adcResultCntr < phCurrents_M1.adcResultCntrMax)
    {
        phCurrents_M1.adcResultSum.Wa += drvGetAdcWa();
        phCurrents_M1.adcResultSum.Va += drvGetAdcVa();
        phCurrents_M1.adcResultSum.Vb += drvGetAdcVb();
        phCurrents_M1.adcResultSum.Ub += drvGetAdcUb();

        if(++phCurrents_M1.adcResultCntr >= phCurrents_M1.adcResultCntrMax)
        {
            phCurrents_M1.brdAdcOffset.Wa = phCurrents_M1.adcResultSum.Wa / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.Va = phCurrents_M1.adcResultSum.Va / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.Vb = phCurrents_M1.adcResultSum.Vb / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.Ub = phCurrents_M1.adcResultSum.Ub / phCurrents_M1.adcResultCntr;
            
            adcOffsetWa = phCurrents_M1.brdAdcOffset.Wa;
            adcOffsetVa = phCurrents_M1.brdAdcOffset.Va;
            adcOffsetVb = phCurrents_M1.brdAdcOffset.Vb;
            adcOffsetUb = phCurrents_M1.brdAdcOffset.Ub;
            
            return true;
        }
    }
    else
    {
        return true;
    }
    
    return false;
#elif defined(RX66T_SONIC)
    if(phCurrents_M1.adcResultCntr < phCurrents_M1.adcResultCntrMax)
    {
        phCurrents_M1.adcResultSum.W1 += drvGetAdcW1();
        phCurrents_M1.adcResultSum.V1 += drvGetAdcV1();
        phCurrents_M1.adcResultSum.V0 += drvGetAdcV0();
        phCurrents_M1.adcResultSum.U0 += drvGetAdcU0();

        if(++phCurrents_M1.adcResultCntr >= phCurrents_M1.adcResultCntrMax)
        {
            phCurrents_M1.brdAdcOffset.W1 = phCurrents_M1.adcResultSum.W1 / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.V1 = phCurrents_M1.adcResultSum.V1 / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.V0 = phCurrents_M1.adcResultSum.V0 / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.U0 = phCurrents_M1.adcResultSum.U0 / phCurrents_M1.adcResultCntr;
            
            adcOffsetW1 = phCurrents_M1.brdAdcOffset.W1;
            adcOffsetV1 = phCurrents_M1.brdAdcOffset.V1;
            adcOffsetV0 = phCurrents_M1.brdAdcOffset.V0;
            adcOffsetU0 = phCurrents_M1.brdAdcOffset.U0;
            
            return true;
        }
    }
    else
    {
        return true;
    }
    
    return false;
    
#elif defined(GD32F303)
    if(phCurrents_M1.adcResultCntr < phCurrents_M1.adcResultCntrMax)
    {
      phCurrents_M1.adcResultSum.W1 += drvGetAdcW1();
      phCurrents_M1.adcResultSum.V1 += drvGetAdcV1();
      phCurrents_M1.adcResultSum.V0 += drvGetAdcV0();
      phCurrents_M1.adcResultSum.U0 += drvGetAdcU0();
      
      if(++phCurrents_M1.adcResultCntr >= phCurrents_M1.adcResultCntrMax)
      {
        phCurrents_M1.brdAdcOffset.W1 = phCurrents_M1.adcResultSum.W1 / phCurrents_M1.adcResultCntr;
        phCurrents_M1.brdAdcOffset.V1 = phCurrents_M1.adcResultSum.V1 / phCurrents_M1.adcResultCntr;
        phCurrents_M1.brdAdcOffset.V0 = phCurrents_M1.adcResultSum.V0 / phCurrents_M1.adcResultCntr;
        phCurrents_M1.brdAdcOffset.U0 = phCurrents_M1.adcResultSum.U0 / phCurrents_M1.adcResultCntr;
        
        adcOffsetW1 = phCurrents_M1.brdAdcOffset.W1;
        adcOffsetV1 = phCurrents_M1.brdAdcOffset.V1;
        adcOffsetV0 = phCurrents_M1.brdAdcOffset.V0;
        adcOffsetU0 = phCurrents_M1.brdAdcOffset.U0;
        
        return true;
      }
    }
    else
    {
      return true;
    }
    
    return false;
#elif defined(RX66T_STORM)
    if(phCurrents_M1.adcResultCntr < phCurrents_M1.adcResultCntrMax)
    {
        phCurrents_M1.adcResultSum.W1 += drvGetAdcW1();
        phCurrents_M1.adcResultSum.V1 += drvGetAdcV1();
        phCurrents_M1.adcResultSum.V0 += drvGetAdcV0();
        phCurrents_M1.adcResultSum.U0 += drvGetAdcU0();

        if(++phCurrents_M1.adcResultCntr >= phCurrents_M1.adcResultCntrMax)
        {
            phCurrents_M1.brdAdcOffset.W1 = phCurrents_M1.adcResultSum.W1 / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.V1 = phCurrents_M1.adcResultSum.V1 / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.V0 = phCurrents_M1.adcResultSum.V0 / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.U0 = phCurrents_M1.adcResultSum.U0 / phCurrents_M1.adcResultCntr;
            
            adcOffsetW1 = phCurrents_M1.brdAdcOffset.W1;
            adcOffsetV1 = phCurrents_M1.brdAdcOffset.V1;
            adcOffsetV0 = phCurrents_M1.brdAdcOffset.V0;
            adcOffsetU0 = phCurrents_M1.brdAdcOffset.U0;
            
            return true;
        }
    }
    else
    {
        return true;
    }
    
    return false;
#elif defined(RX13T)
    if(phCurrents_M1.adcResultCntr <= phCurrents_M1.adcResultCntrMax)
    {
    	phCurrents_M1.adcResultSum.W1 += drvGetAdcW1();
    	phCurrents_M1.adcResultSum.V0 += drvGetAdcV0();
    	phCurrents_M1.adcResultSum.U0 += drvGetAdcU0();
        if(++phCurrents_M1.adcResultCntr >= phCurrents_M1.adcResultCntrMax)
        {
        	phCurrents_M1.brdAdcOffset.W1 = phCurrents_M1.adcResultSum.W1 / phCurrents_M1.adcResultCntr;
        	phCurrents_M1.brdAdcOffset.V0 = phCurrents_M1.adcResultSum.V0 / phCurrents_M1.adcResultCntr;
        	phCurrents_M1.brdAdcOffset.U0 = phCurrents_M1.adcResultSum.U0 / phCurrents_M1.adcResultCntr;

            adcOffsetW1 = phCurrents_M1.brdAdcOffset.W1;
            adcOffsetV0 = phCurrents_M1.brdAdcOffset.V0;
            adcOffsetU0 = phCurrents_M1.brdAdcOffset.U0;

                return true;
        }
    }
    else
    {
    	return true;
    }
    return false;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdCurrentCalibrationHandler = true;
    return true;
#elif defined(VISUAL_STUDIO)
    printf("brdCurrentCalibrationHandler_M1\n");
    vsbrdCurrentCalibrationHandler = true;
    return true;
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
	if(phCurrents_M1.adcResultCntr < phCurrents_M1.adcResultCntrMax)
    {
        phCurrents_M1.adcResultSum.Wa += drvGetAdcWa(); //ARDA //EFE01 ADC->RSLT[0];
        phCurrents_M1.adcResultSum.Va += drvGetAdcVa(); //EFE01 ADC->RSLT[1];
        //phCurrents_M1.adcResultSum.Vb += drvGetAdcVb(); //EFE01 ADC->RSLT[8];
        phCurrents_M1.adcResultSum.Ub += drvGetAdcUb(); //EFE01 ADC->RSLT[9];

        if(++phCurrents_M1.adcResultCntr >= phCurrents_M1.adcResultCntrMax)
        {
            phCurrents_M1.brdAdcOffset.Wa = phCurrents_M1.adcResultSum.Wa / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.Va = phCurrents_M1.adcResultSum.Va / phCurrents_M1.adcResultCntr;
           // phCurrents_M1.brdAdcOffset.Vb = phCurrents_M1.adcResultSum.Vb / phCurrents_M1.adcResultCntr;
            phCurrents_M1.brdAdcOffset.Ub = phCurrents_M1.adcResultSum.Ub / phCurrents_M1.adcResultCntr;
            
            adcOffsetWa = phCurrents_M1.brdAdcOffset.Wa;
            adcOffsetVa = phCurrents_M1.brdAdcOffset.Va;
            adcOffsetVb = phCurrents_M1.brdAdcOffset.Vb;
            adcOffsetUb = phCurrents_M1.brdAdcOffset.Ub;
            
            return true;
        }
    }
    else
    {
        return true;
    }
    
    return false;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
bool brdCurrentCalibrationHandler_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(RX66T_STORM)
	 if(phCurrents_M2.adcResultCntr < phCurrents_M2.adcResultCntrMax)
	    {
	        phCurrents_M2.adcResultSum.W1 += drvGetAdcW1_M2();
	        phCurrents_M2.adcResultSum.V1 += drvGetAdcV1_M2();
	        phCurrents_M2.adcResultSum.V0 += drvGetAdcV0_M2();
	        phCurrents_M2.adcResultSum.U0 += drvGetAdcU0_M2();

	        if(++phCurrents_M2.adcResultCntr >= phCurrents_M2.adcResultCntrMax)
	        {
	            phCurrents_M2.brdAdcOffset.W1 = phCurrents_M2.adcResultSum.W1 / phCurrents_M2.adcResultCntr;
	            phCurrents_M2.brdAdcOffset.V1 = phCurrents_M2.adcResultSum.V1 / phCurrents_M2.adcResultCntr;
	            phCurrents_M2.brdAdcOffset.V0 = phCurrents_M2.adcResultSum.V0 / phCurrents_M2.adcResultCntr;
	            phCurrents_M2.brdAdcOffset.U0 = phCurrents_M2.adcResultSum.U0 / phCurrents_M2.adcResultCntr;

	            adcOffsetW1_M2 = phCurrents_M2.brdAdcOffset.W1;
	            adcOffsetV1_M2 = phCurrents_M2.brdAdcOffset.V1;
	            adcOffsetV0_M2 = phCurrents_M2.brdAdcOffset.V0;
	            adcOffsetU0_M2 = phCurrents_M2.brdAdcOffset.U0;

	            return true;
	        }
	    }
	    else
	    {
	        return true;
	    }

	    return false;
#elif defined(QT_CPP_SIMULATOR)
    return true;
    qt_brdCurrentCalibrationHandler = true;
#elif defined(VISUAL_STUDIO)
    printf("brdCurrentCalibrationHandler_M2\n");
    vsbrdCurrentCalibrationHandler = true;
    return true;
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
if(phCurrents_M2.adcResultCntr < phCurrents_M2.adcResultCntrMax)
    {
        phCurrents_M2.adcResultSum.Wa += drvGetAdcWa_M2(); //ARDA //EFE01 ADC->RSLT[0];
        phCurrents_M2.adcResultSum.Va += drvGetAdcVa_M2(); //EFE01 ADC->RSLT[1];
       // phCurrents_M2.adcResultSum.Vb += drvGetAdcVb_M2(); //EFE01 ADC->RSLT[8];
        phCurrents_M2.adcResultSum.Ub += drvGetAdcUb_M2(); //EFE01 ADC->RSLT[9];

        if(++phCurrents_M2.adcResultCntr >= phCurrents_M2.adcResultCntrMax)
        {
            phCurrents_M2.brdAdcOffset.Wa = phCurrents_M2.adcResultSum.Wa / phCurrents_M2.adcResultCntr;
            phCurrents_M2.brdAdcOffset.Va = phCurrents_M2.adcResultSum.Va / phCurrents_M2.adcResultCntr;
          //  phCurrents_M2.brdAdcOffset.Vb = phCurrents_M2.adcResultSum.Vb / phCurrents_M2.adcResultCntr;
            phCurrents_M2.brdAdcOffset.Ub = phCurrents_M2.adcResultSum.Ub / phCurrents_M2.adcResultCntr;
            
            adcOffsetWa_M2 = phCurrents_M2.brdAdcOffset.Wa;
            adcOffsetVa_M2 = phCurrents_M2.brdAdcOffset.Va;
            adcOffsetVb_M2 = phCurrents_M2.brdAdcOffset.Vb;
            adcOffsetUb_M2 = phCurrents_M2.brdAdcOffset.Ub;
            
            return true;
        }
    }
    else
    {
        return true;
    }
    
    return false;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
float brdGetIpmTemperature_M1(void)
{
#if defined(MKV42F16)
  return ipmTemperature_M1;
#elif defined(RX66T_SONIC)
  return 0;	//ipmTemperature_M1;
#elif defined(RX13T)
  return 0.0f;
#elif defined(GD32F303)
  return GetIpmTemp();
#elif defined(RX66T_STORM)
    return ipmTemperature_M1;
#elif defined(RX66T_AIRCON) || defined(MCXA166_DW)
    return ipmTemperature_M1;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetIpmTemperature = true;
    return 0.0f;
#elif defined(VISUAL_STUDIO)
    printf("brdGetIpmFaultOutState_M1\n");
    vsbrdGetIpmTemperature = true;
    return 0.0f;
#elif defined(RX24T)
    return ipmTemperature_M1;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
float brdGetIpmTemperature_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)
    return 0.0f;
#elif defined(RX66T_STORM)
	return ipmTemperature_M2;
#elif defined(RX66T_AIRCON) || defined(MCXA166_DW)
	return ipmTemperature_M2;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetIpmTemperature = true;
    return 0.0f;
#elif defined(VISUAL_STUDIO)
    printf("brdGetIpmFaultOutState_M2\n");
    vsbrdGetIpmTemperature = true;
    return 0.0f;
#elif defined(RX24T)
	return ipmTemperature_M2;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
bool brdGetIpmFaultOutState_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(GD32F303)
    return ipmFaultOutHandled;
#elif defined(RX66T_STORM)
    return ipmFaultOutHandled;
#elif defined(RX13T)
	drvGetIPMFaultState();
    return ipmFaultOutHandled;
#elif defined(RX66T_AIRCON) || defined(MCXA166_DW)
    return ipmFaultOutHandled_M1;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetIpmFaultOutState = true;
    return false;
#elif defined(VISUAL_STUDIO)
    printf("brdGetIpmFaultOutState_M1\n");
    vsbrdGetIpmFaultOutState = true;
    return false;
#elif defined(RX24T)

#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
bool brdGetIpmFaultOutState_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)
    return false;
#elif defined(RX66T_STORM)
	return false;
#elif defined(RX66T_AIRCON) || defined(MCXA166_DW)
	return ipmFaultOutHandled_M2;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdGetIpmFaultOutState = true;
    return false;
#elif defined(VISUAL_STUDIO)
    printf("brdGetIpmFaultOutState_M2\n");
    vsbrdGetIpmFaultOutState = true;
    return false;
#elif defined(RX24T)
	return false;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
void brdResetIpmFaultOutState_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T) || defined(GD32F303)
    ipmFaultOutHandled = false;
#elif defined(RX66T_STORM)
    ipmFaultOutHandled = false;
#elif defined(RX66T_AIRCON) || defined(MCXA166_DW)
    ipmFaultOutHandled_M1 = false;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdResetIpmFaultOutState = true;
#elif defined(VISUAL_STUDIO)
    printf("brdResetIpmFaultOutState_M1\n");
    vsbrdResetIpmFaultOutState = true;
#elif defined(RX24T)

#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdResetIpmFaultOutState_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(RX66T_STORM)
	return false;
#elif defined(RX66T_AIRCON) || defined(MCXA166_DW)
    ipmFaultOutHandled_M2 = false;
#elif defined(QT_CPP_SIMULATOR)
    qt_brdResetIpmFaultOutState = true;
#elif defined(VISUAL_STUDIO)
    printf("brdResetIpmFaultOutState_M2\n");
    vsbrdResetIpmFaultOutState = true;
#elif defined(RX24T)

#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
void brdPwmEnable_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    drvPwmEnable();
#elif defined(RX66T_STORM)
    drvPwmEnable();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnable = true;
#elif defined(VISUAL_STUDIO)
    vsbrdPwmEnable = true;
    printf("brdPwmEnable_M1\n");
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdPwmEnable_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmEnable = true;
#elif defined(VISUAL_STUDIO)
    vsbrdPwmEnable = true;
    printf("brdPwmEnable_M2\n");
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    drvPwmEnable_M2();
#elif defined(RX66T_STORM)
    drvPwmEnable_M2();
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
void brdPwmDisable_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    drvPwmDisable();
#elif defined(RX66T_STORM)
    drvPwmDisable();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmDisable = true;
#elif defined(VISUAL_STUDIO)
    vsbrdPwmDisable = true;
    printf("brdPwmDisable_M1\n");
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdPwmDisable_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T)

#elif defined(QT_CPP_SIMULATOR)
    qt_brdPwmDisable = true;
#elif defined(VISUAL_STUDIO)
    vsbrdPwmDisable = true;
    printf("brdPwmDisable_M2\n");
#elif defined(RX24T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    drvPwmDisable_M2();
#elif defined(RX66T_STORM)
    drvPwmDisable_M2();
#else
#error Process environment should be defined;
#endif
}
#endif

void brdWatchdogResetHandlerFL(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    drvWatchdogResetHandlerFL();
#elif defined(RX66T_STORM)
    drvWatchdogResetHandlerFL();
#elif defined(RX13T)
    
#elif defined(QT_CPP_SIMULATOR)
    qt_brdWatchdogResetHandlerFL = true;
#elif defined(VISUAL_STUDIO)
    vsbrdWatchdogResetHandlerFL = true;
    printf("brdWatchdogResetHandlerFL\n");
#else
#error Process environment should be defined;
#endif
}

void brdResetWdg(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    drvResetWdg();
#elif defined(RX66T_STORM)
    drvResetWdg();
#elif defined(RX13T)
    
#elif defined(QT_CPP_SIMULATOR)
    qt_brdResetWdg = true;
#elif defined(VISUAL_STUDIO)
    vsbrdResetWdg = true;
    printf("brdResetWdg\n");
#else
#error Process environment should be defined;
#endif
}

void brdEnableInterrupts(void)
{
#if defined(MKV42F16) || defined(RX24T) || defined(RX66T_AIRCON) || defined(MCXA166_DW)
  drvEnableInterrupts();
  drvWatchdogEnable();
#elif defined(RX66T_SONIC)
  drvEnableInterrupts();
#elif defined(RX13T)
  drvEnableInterrupts();
#elif defined(GD32F303)
  drvEnableInterrupts();
#elif defined(RX66T_STORM)
    drvEnableInterrupts();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdEnableInterrupts = true;
#elif defined(VISUAL_STUDIO)
    vsbrdEnableInterrupts = true;
    printf("brdEnableInterrupts\n");
#else
#error Process environment should be defined;
#endif
}

void brdDisableInterrupts(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    drvDisableInterrupts();
#elif defined(RX66T_STORM)
    drvDisableInterrupts();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdDisableInterrupts = true;
#elif defined(VISUAL_STUDIO)
    vsbrdDisableInterrupts = true;
    printf("brdDisableInterrupts\n");
#else
#error Process environment should be defined;
#endif
}


bool brdGetDCBusOpenStatus(void)
{
#if defined(MKV42F16) || defined(RX66T_AIRCON) || defined(RX13T) || defined(MCXA166_DW)
    return false;
#elif defined(GD32F303)
    return drvGetDcBusOpenStatus();
#elif defined(RX66T_STORM)
    return false;
#elif defined(QT_CPP_SIMULATOR)
    *_pos = encoder_pos;
    *_sp = encoder_sp;
    qt_brdGetEncoderData = true;
    return true;
#elif defined(RX24T)
    return false;
#elif defined(VISUAL_STUDIO)
    *_pos = encoder_pos;
    *_sp = encoder_sp;
    vsbrdGetEncoderData = TRUE;
    printf("brdGetEncoderData_M1 --> encoder_pos = %f, encoder_sp = %f\n", *_pos, *_sp);
    return TRUE;
#else
#error Process environment should be defined;
#endif
}

#ifdef MC_ENABLE_FIRST_MOTOR
bool brdGetEncoderData_M1(float *_pos, float *_sp)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX13T) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    return false;
#elif defined(RX66T_STORM)
    return false;
#elif defined(QT_CPP_SIMULATOR)
    *_pos = encoder_pos;
    *_sp = encoder_sp;
    qt_brdGetEncoderData = true;
    return true;
#elif defined(RX24T)
    return false;
#elif defined(VISUAL_STUDIO)
    *_pos = encoder_pos;
    *_sp = encoder_sp;
    vsbrdGetEncoderData = TRUE;
    printf("brdGetEncoderData_M1 --> encoder_pos = %f, encoder_sp = %f\n", *_pos, *_sp);
    return TRUE;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
bool brdGetEncoderData_M2(float *_pos, float *_sp)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX66T_AIRCON) || defined(GD32F303) || defined(MCXA166_DW)
    return false;
#elif defined(RX66T_STORM)
    return false;
#elif defined(QT_CPP_SIMULATOR)
    *_pos = encoder_pos;
    *_sp = encoder_sp;
    qt_brdGetEncoderData = true;
    return true;
#elif defined(VISUAL_STUDIO)
    *_pos = encoder_pos;
    *_sp = encoder_sp;
    vsbrdGetEncoderData = TRUE;
    printf("brdGetEncoderData_M2 --> encoder_pos = %f, encoder_sp = %f\n", *_pos, *_sp);
    return TRUE;
#elif defined(RX24T)
    return false;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_FIRST_MOTOR
void brdShortCircuitBottomTransistors_M1(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX66T_AIRCON) || defined(GD32F303)
    drvShortCircuitBottomTransistors();
#elif defined(RX66T_STORM)
    //drvShortCircuitBottomTransistors();
#elif defined(RX13T) || defined(MCXA166_DW)

#elif defined(QT_CPP_SIMULATOR)
    qt_brdShortCircuitBottomTransistors_M1 = true;
#elif defined(VISUAL_STUDIO)
    vs_brdShortCircuitBottomTransistors_M1 = true;
#else
#error Process environment should be defined;
#endif
}
#endif

#ifdef MC_ENABLE_SECOND_MOTOR
void brdShortCircuitBottomTransistors_M2(void)
{
#if defined(MKV42F16) || defined(RX66T_SONIC) || defined(RX24T) || defined(RX13T) || defined(MCXA166_DW)

#elif defined(RX66T_STORM)
    //drvShortCircuitBottomTransistors_M2();
#elif defined(QT_CPP_SIMULATOR)
    qt_brdShortCircuitBottomTransistors_M2 = true;
#elif defined(VISUAL_STUDIO)
    vs_brdShortCircuitBottomTransistors_M2 = true;
#elif defined(RX66T_AIRCON)
	//drvShortCircuitBottomTransistors_M2();
#else
#error Process environment should be defined;
#endif
}

#ifdef RX66T_AIRCON
float brdSampleCompressorTemp(void)
{
    return (((float)Temp_NTC_Discharge) / 10.0f);	
}
float brdSampleCondenserTemp(void)
{
    return (((float)Temp_NTC_Condenser) / 10.0f);	
}
float brdSampleAmbientTemp(void)
{
    return (((float)Temp_NTC_Ambient) / 10.0f);	
}
float brdSampleCompressorIpmTemp(void)
{
    return Temp_NTC_CompIPM;	
}
float brdSampleFanIpmTemp(void)
{
    return Temp_NTC_FanIPM;	
}
void brdPcbLedOn(void)
{
    drvPcbLedOn();
}
void brdPcbLedOff(void)
{
    drvPcbLedOff();
}
void brdPcbLedToggle(void)
{
    drvPcbLedToggle();
}
void brdRead4Wayon(void)
{
    drvRead4Wayon();
}
void brdRead4Wayoff(void)
{
    drvRead4Wayoff();    
}	
void brdStepMotorAset(void)
{
    drvStepMotorAset();
}
void brdStepMotorBset(void)
{
    drvStepMotorBset();
}
void brdStepMotorCset(void)
{
    drvStepMotorCset();
}
void brdStepMotorDset(void)
{
    drvStepMotorDset();
}
void brdStepMotorAclear(void)
{
    drvStepMotorAclear();
}
void brdStepMotorBclear(void)
{
    drvStepMotorBclear();
}
void brdStepMotorCclear(void)
{
    drvStepMotorCclear();
}
void brdStepMotorDclear(void)
{
    drvStepMotorDclear();
}

void brdHighFanSet(void)
{
    drvHighFanSet();
}
void brdHighFanClear(void)
{
    drvHighFanClear();
}
void brdMidFanSet(void)
{
    drvMidFanSet();
}
void brdMidFanClear(void)
{
    drvMidFanClear();
}

unsigned char brdCompOvlPort(void){  return 0;}
unsigned char brdIpmProtectPort(void){  return 0;}
unsigned char brdDcFanFaultPort(void){  return 0;}
unsigned char brdPfcProtectPort(void){  return 0;}

void brdIpmBrkFlgClear(void){   1;  }
void brdDcFanBrkFlgClear(void){ 1;  }
void brdPfcBrkFlgClear(void){   1;}

#endif

#ifdef MCXA166_DW

void brdPumpSet(void)
{
	drvPumpSet();
}

void brdPumpReset(void)
{
	drvPumpReset();
}


void brdDispenserSet(void)
{
	drvDispenserSet();
}

void brdDispenserReset(void)
{
	drvDispenserReset();
}
void brdRegenSet(void)
{
	drvRegenSet();
}
void brdRegenReset(void)
{
	drvRegenReset();
}
void brdH2OSet(void)
{
	drvH2OSet();
}
void brdH2OReset(void)
{
	drvH2OReset();
}
void brdDiverterSet(void)
{
	drvDiverterSet();
}
void brdDiverterReset(void)
{
	drvDiverterReset();
}
void brdRelayHeaterSet(void)
{
	drvRelayHeaterSet();
}
void brdRelayHeaterReset(void)
{
	drvRelayHeaterReset();
}
void brdSwitchHeaterSet(void)
{
	drvSwitchHeaterSet();
}
void brdSwitchHeaterReset(void)
{
	drvSwitchHeaterReset();
}

float brdGetTempIPM_M1(void)
{
	drvGetTempIPM_M1();
}
float brdGetTempIPM_M2(void)
{
	drvGetTempIPM_M2();
}



#endif

#endif

void brdHandlerML(void)
{
#if defined(GD32F303)
    test_run_cycle();
#endif
        
    brdResetWdg();
}

