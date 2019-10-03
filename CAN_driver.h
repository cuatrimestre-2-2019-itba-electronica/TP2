void CAN_RESET(void);
void CAN_BIT_MODIFY(char address, char mask, char data);
void CAN_WRITE(char address, char * buffer, int bufflen);
void CAN_READ(char address, char * buffer, int bufflen);
void CAN_RTS(char address, char * buffer, int bufflen);
void init_CAN(int ID);
void send_CAN(int ID,char * buffer, int bufflen);
