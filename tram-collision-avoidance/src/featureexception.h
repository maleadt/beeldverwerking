//
// Configuration
//

// Include guard
#ifndef FEATUREEXCEPTION_H
#define FEATUREEXCEPTION_H

// Includes
#include <exception>
#include <QString>

class FeatureException : public std::exception
{
public:
    FeatureException(const char* iMessage) throw() : mMessage(iMessage)
    {
    }

    FeatureException(const std::string& iMessage) throw() : mMessage(iMessage)
    {
    }

    FeatureException(const QString &iMessage) throw() : mMessage(iMessage.toStdString())
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
