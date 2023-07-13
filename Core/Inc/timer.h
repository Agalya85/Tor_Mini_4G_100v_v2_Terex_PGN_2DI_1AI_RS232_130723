/*
 * timer.h
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

//#define FIVEHUNDRED_US 	(20)
//
//#define ONE_MS 			(40)

#define ONE_MS 			(1)
#define FIVE_MS 		(5 * ONE_MS)
#define TEN_MS 			(2 * FIVE_MS)
#define TWENTY_MS 		(4 * FIVE_MS)

#define THIRTY_MS_SEC	(3 * TEN_MS)
#define FIFTY_MS		(5 * TEN_MS)
#define SIXTY_TWO_MS 	(62 * ONE_MS)
#define HUNDRED_MS		(100 * ONE_MS)
#define FIVEHUNDRED_MS 	(100 * FIVE_MS)

#define ONE_SEC 		(1000 * ONE_MS)
#define TWO_SEC  		(2 * ONE_SEC)
#define FOUR_SEC 		(2 * TWO_SEC)
#define FIVE_SEC		(5 * ONE_SEC)
#define SEVEN_SEC		(7 * ONE_SEC)
#define TEN_SEC			(2 * FIVE_SEC)
#define FIFTEEN_SEC		(3 * FIVE_SEC)
#define TWENTY_SEC		(4 * FIVE_SEC)
#define TWENTY_FIVE		(5 * FIVE_SEC)
#define THIRTY_SEC		(3 * TEN_SEC)

#define ONE_MIN			(6 * TEN_SEC)
#define TWO_MIN			(2 * ONE_MIN)
#define THREE_MIN		(3 * ONE_MIN)
#define FIVE_MIN		(5 * ONE_MIN)
#define TEN_MIN			(2 * FIVE_MIN)


#endif /* INC_TIMER_H_ */
