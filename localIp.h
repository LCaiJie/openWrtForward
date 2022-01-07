#include <netinet/in.h>

/**
 * @brief  本地IP更新
 * @return 
 */
void localIpUpdate(void);

/**
 * @brief  获取本地IP的数量
 * @return 存在的IP
 */
int localIpNum(void);

/**
 * @brief  获取本地IP
 * @param  index 按照位置查找
 * @return ip地址
 */
in_addr_t localIpGet(int index);

/**
 * @brief  判断IP是否是本地IP，返回0是本地IP
 * @return 0 是本地IP
 */
int localIpJudge(in_addr_t ip);

