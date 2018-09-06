/*
 * RoadPassion CONFIDENTIAL
 *
 * Copyright 2013 RoadPassion Electronics Co., Ltd.
 *
 * DO NOT COPY AND/OR REDISTRIBUTE WITHOUT PERMISSION.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef  __TASK_PRIO_H__
#define  __TASK_PRIO_H__

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define GRTKEY_TASK_PRIO				5
#define CAN_HUB_TASK_PRIO				20
#define CAN_TP_20_MONITOR_TASK_PRIO			21 //30
#define CAN_TP_20_IDLE_HANDLER_TASK_PRIO                22//31
#define ENABLED_CAN_TASK_PRIO   			23//59
#define CAR_TASK_PRIO					24//55
#define COM_KEEPING_TASK_LISTENING_TASK_PRIO	        25//44
#define COM_KEEPING_TASK_STIM_TASK_PRIO		        26//45
#define RANDOM_MSG_TASK_PRIO				27//46
#define TIREKEYT_SIM_TASK_PRIO				29//33
#define CAR_SPEED_SIM_TASK_PRIO           		30
#define TURN_SIM_TASK_PRIO				31//38
#define TURNING_LIGHT_SIM_TASK_PRIO			32//39
#define STEERING_WHEEL_SIM_TASK_PRIO                    33//40
#define RADAR_SIM_TASK_PRIO				34//41
#define SHIFT_SIM_TASK_PRIO                             35//42
#define P_KEY_SIM_TASK_PRIO                             36//43
#define FRONT_DOOR_KEY_TASK_PRIO    			37//56
#define REMOTE_KEY_TASK_PRIO    			38//57
#define BACK_DOOR_STATUS_TASK_PRIO    			39//58

#define X360_SIM_TASK_PRIO				51//32
#define CAR_LIGHT_TASK_PRIO                      	44
#define JAGUAR_LR_TASK_PRIO                      	52
#define AUTOCONTROL_TASK_PRIO			        50//47

#define MMIKEY_SIM_TASK_PRIO                            40//48
#define BACKLIGHT_SIM_TASK_PRIO                         41//49
#define PAGETRUN_SIM_TASK_PRIO				42//50

#define CARTIME_TASK_PRIO 				43//4

#endif
