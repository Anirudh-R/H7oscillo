/**
  ******************************************************************************
  * @file    params.h
  * @author  anirudhr
  * @brief   Contains global parameter macros.
  ******************************************************************************
  */

#ifndef __PARAMS_H
#define __PARAMS_H

#define CHANNEL1			 1			/* Input channels */
#define CHANNEL2			 2

#define TOFF_LIMIT			 60			/* horizontal offset limit (on both sides of screen) */

#define ADC_TRIGBUF_SIZE	 480								/* Size of ADC buffer used for triggering and measurements */
#define ADC_PRETRIGBUF_SIZE	 ADC_TRIGBUF_SIZE - TOFF_LIMIT		/* Pre-trigger buffer size */
#define ADC_POSTRIGBUF_SIZE	 ADC_TRIGBUF_SIZE - TOFF_LIMIT		/* Post-trigger buffer size */
#define	ADC_BUF_SIZE		 ADC_TRIGBUF_SIZE + \
							  ADC_PRETRIGBUF_SIZE + \
							  ADC_POSTRIGBUF_SIZE				/* Size of ADC sampling buffer */

#define UGUI_MAX_OBJECTS     10			/* uGUI max objects in a window */

#endif /* __PARAMS_H */
