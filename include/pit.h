/*****************************************************************************
 * PIT - BASE
 ****************************************************************************/
 
#define PIT_TIMER_LOADv_VAL	0xFFFFFFFF
#define MOD_CLK_PIT0			(1<<5)
#define MOD_CLK_PIT1			(1<<6)

#define PIT_NUM_0	SOC_PIT0_BASE
#define PIT_NUM_1	SOC_PIT1_BASE


#define PIT_MODE_TIMER		0
#define PIT_MODE_PWM		1



#define PIT_CHN_0	0
#define PIT_CHN_1	1
#define PIT_CHN_2	2
#define PIT_CHN_3	3
/************************************
 *  TARGET PIT CONSTANTS            *
 ************************************/
//todo
#define PIT_IDREV_OFFSET        0x00    /* Offset for PIT ID and revision register */
#define PIT_CONFIG_OFFSET       0x10    /* Offset for PIT configure register */
#define PIT_INTE_OFFSET         0x14    /* Offset for PIT interrupt enable counter */
#define PIT_INTS_OFFSET         0x18    /* Offset for PIT interrupt status counter */
#define PIT_CHNEN_OFFSET        0x1C    /* Offset for PIT channel enable register */
#define PIT_CTRL_CHN(N)         (0x20 + 0x10 * (N))
#define PIT_LOAD_CHN(N)         (0x24 + 0x10 * (N))
#define PIT_CNTR_CHN(N)         (0x28 + 0x10 * (N))

/* PIT Channel 0 ~ 3 as Timer 0 ~ 3, ATCPIT100 */
#define TM_IE                   (PIT_BASE + PIT_INTE_OFFSET)
#define TM_STA                  (PIT_BASE + PIT_INTS_OFFSET)
#define TM_CHNEN                (PIT_BASE + PIT_CHNEN_OFFSET)
#define TM_CTRL(N)              (PIT_BASE + PIT_CTRL_CHN(N))
#define TM_LOAD(N)              (PIT_BASE + PIT_LOAD_CHN(N))
#define TM_CNTR(N)              (PIT_BASE + PIT_CNTR_CHN(N))

#define TM_CTRL_32BIT           (0x1 << 0)
#define TM_CTRL_PCLK            (0x1 << 3)

unsigned int pit_ch_count_get(unsigned int pit_base, unsigned int num);

//int pit_ch_reload_value(unsigned int pit_base, unsigned int num, unsigned int value);

#define PIT_CHCLK_EXTERN	0
#define PIT_CHCLK_APB		1

#define PIT_CHMODE_RES1		0
#define PIT_CHMODE_32BIT_TM	1
#define PIT_CHMODE_16BIT_TM	2
#define PIT_CHMODE_8BIT_TM		3
#define PIT_CHMODE_PWM		4
#define PIT_CHMODE_RES2		5
#define PIT_CHMODE_MIXED_PWM_16BIT_TM	6
#define PIT_CHMODE_MIXED_PWM_8BIT_TM	7
//int pit_ch_ctrl(unsigned int pit_base, unsigned int num, unsigned int PWMPark, unsigned int chClk, unsigned int chMode);


#define PIT_CH_TM0_EN			0x1
#define PIT_CH_TM1_EN			0x2
#define PIT_CH_TM2_EN			0x4
#define PIT_CH_TM3_PWM_EN		0x8
#define PIT_CH_DISABLE			0x0
//int pit_ch_mode_set(unsigned int pit_base, unsigned int num, unsigned int enable);
//int pit_ch_mode_enable(unsigned int pit_base, unsigned int num, unsigned int enable);

#define PIT_INT_STATUS_CLEAN		1
#define PIT_INT_STATUS_GET			0
//int pit_int_status_handle(unsigned int pit_base, unsigned int num, unsigned int action);

//int pit_int_enable(unsigned int pit_base, unsigned int num, unsigned int enable);

void pit_init(unsigned int pit_base);

void pit_delay(unsigned long delay);
void udelay(unsigned int delay);

//#define mdelay(x)	pit_delay((x)*100)

