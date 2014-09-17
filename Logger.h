#pragma once

#include <fstream>

#define LOG_ENABLED false
#define LOG_FILE_NAME "log.txt"

#define LOG Logger::log()

class Logger{
public:
	Logger(){
		outputFile.open (LOG_FILE_NAME, std::ios::out | std::ios::app);
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
        outputFile << data << std::endl;
		return log();
    }

private:
	std::ofstream outputFile;
};