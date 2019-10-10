/*----------------------------------------------------------------------------
 * Description:   Methods for bridging to an external entity.
 *
 * External Entity:  Tello (Tello)
 * 
 * your copyright statement can go here (from te_copyright.body)
 *--------------------------------------------------------------------------*/

#include "telloctrl_sys_types.h"
#include "Tello_bridge.h"
#include "TIM_bridge.h"
#include "udp.h"

/*
 * Bridge:  send
 */
void
Tello_send( c_t p_str[ESCHER_SYS_MAX_STRING_LEN] )
{
	udp_send(p_str);
}

