#ifndef LASERLINK_H
#define LASERLINK_H

enum laserEnum {
    PAR, //Parameter change request
    HBT, //Heartbeat
    PRQ, //Request parameters
    PSE, //send parameters
    MOV, //move the gimbal manually in x, y
    DOC}; //Do command

enum do_what {
    MOD,
    TOG};

int parse_header(std::string inHeader, laserEnum* messageEnum);

class laserlink
{
    public:

        std::string header_raw;
        
        int body_length;
        std::string body_raw;
       
        int check_length();

        bool isValid;

    private:
};

class ll_parameters: public laserlink
{
    public:
        

    private:
};

class ll_do: public laserlink
{
    public:
        ll_do(std::string inMessage, int bodylen);
        do_what command;
        int param1;

        void execute_command(void);
    private:
};

class ll_mov: public laserlink
{
    public:
        ll_mov(std::string inMessage, int bodylen);
        int xMov;
        int yMov;

        void execute_command(void);
    private:
};

#endif
