#ifndef SIGNALSHANDLER_H
#define SIGNALSHANDLER_H

class SignalsHandler
{
public:
	SignalsHandler();
	~SignalsHandler();

    void startup();
    void shutdown();

	void wait_SIGINT(void);
	void wait_PMAN(void);

private:
};

//extern SignalsHandler signalsHandler;

#endif
