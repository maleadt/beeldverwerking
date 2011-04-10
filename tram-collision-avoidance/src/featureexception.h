//
// Configuration
//

// Include guard
#ifndef FEATUREEXCEPTION_H
#define FEATUREEXCEPTION_H

// Includes
#include <exception>

class FeatureException : public std::exception
{
public:
    FeatureException(const std::string& iMessage) throw() : mMessage(iMessage)
    {
    }
    ~FeatureException() throw() {}

    virtual const char* what() const throw()
    {
      return mMessage.c_str();
    }

private:
    std::string mMessage;
};

#endif // FEATUREEXCEPTION_H
