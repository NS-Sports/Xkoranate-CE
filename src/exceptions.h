#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QString>

#include <stdexcept>
#include <string>

class XkorFileNotFoundException : public std::logic_error
{
	private:
		QString name;
		QString type;
	public:
		XkorFileNotFoundException(std::string errorText, QString fileType, QString fileName);
		~XkorFileNotFoundException() noexcept {;;;}
		QString fileType();
		QString fileName();
};

class XkorSearchFailedException : public std::logic_error
{
	public:
		XkorSearchFailedException(std::string errortext) : std::logic_error(errortext) {;;;}
		~XkorSearchFailedException() noexcept {;;;}
};

#endif
