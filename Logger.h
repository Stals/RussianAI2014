#pragma once

#include <fstream>

#define LOG_ENABLED
#define LOG_FILE_NAME "log.txt"

#define LOG Logger::log()

class Logger{
public:
	Logger(){
#ifdef LOG_ENABLED
		outputFile.open (LOG_FILE_NAME, std::ios::out | std::ios::app);
#endif
	}

	static Logger& log(){
		static Logger logger;
		return logger;
	}
	/*
	template <typename T>
	Logger& operator <<(Logger& log, T const& value) {
    if(LOG_ENABLED){		
		outputFile << value;
	}
    
	return log;
}*/
	template<typename T> 
    Logger& operator<< (const T& data) 
    {
#ifdef LOG_ENABLED
        outputFile << data << std::endl;
		return log();
#endif
    }

private:
	std::ofstream outputFile;
};