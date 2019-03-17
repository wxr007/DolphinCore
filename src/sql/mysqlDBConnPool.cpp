#include "sql/sqlHelper.h"
#include<string.h>
#include "serialize/easyStrParse.h"

stUrlInfo::stSqlTypeInfo stUrlInfo::zSqlTypes[] =
{
	{"mssql://", stUrlInfo::eMsSql, 16, CSqlBase::newInstance},
	{"msmdb://", stUrlInfo::eMsMdb, 16, CSqlBase::newInstance},
	{"mysql://", stUrlInfo::eMySql, 16, CSqlBase::newInstance},
	{NULL, stUrlInfo::eTypeError, 0},
};

void stUrlInfo::parseURLString()
{
	FUNCTION_BEGIN;
	urlerror = false;
	ZeroMemory(host, sizeof(host));
	ZeroMemory(user, sizeof(user));
	ZeroMemory(passwd, sizeof(passwd));
	port = 3306;
	ZeroMemory(dbName, sizeof(dbName));
	ZeroMemory(dbConnParam, sizeof(dbConnParam));
	const char *connString = parseurlhead(url.c_str());

	if(connString)
	{
		char strPort[64];
		int  j = 0, k = 0;
		size_t i = 0;

		while(i < strlen(connString))
		{
			switch(j)
			{
			case 0:

				if(connString[i] == ':')
				{
					user[k] = '\0';
					j++;
					k = 0;
				}
				else
				{
					user[k++] = connString[i];
				}

				break;
			case 1:

				if(connString[i] == '@')
				{
					passwd[k] = '\0';
					j++;
					k = 0;
				}
				else
				{
					passwd[k++] = connString[i];
				}

				break;
			case 2:

				if(connString[i] == ':')
				{
					host[k] = '\0';
					j++;
					k = 0;
				}
				else
				{
					host[k++] = connString[i];
				}

				break;
			case 3:

				if(connString[i] == '/')
				{
					strPort[k] = '\0';
					j++;
					k = 0;
				}
				else
				{
					strPort[k++] = connString[i];
				}

				break;
			case 4:
				{
					CEasyStrParse parse;
					parse.SetParseStrEx(&connString[i],(char*) "\"/",(char*) "\"\"", '"');

					if(parse.ParamCount() > 0 && parse.ParamCount() <= 2)
					{
						strcpy_q(dbName, sizeof(dbName) - 1, parse[0]);

						if(parse.ParamCount() > 1)
						{
							strcpy_q(dbConnParam, sizeof(dbConnParam) - 1, parse[1]);
						}
					}
					else
					{
						urlerror = true;
					}

					i = strlen(connString);
				}
				break;
			default:
				break;
			}

			i++;
		}

		if(j != 4)
		{
			urlerror = true;
		}

		port = atoi(strPort);
		return;
	}

	urlerror = true;
}


const char* stUrlInfo::parseurlhead(const char* connstr)
{
	FUNCTION_BEGIN;
	int i = 0;

	while(zSqlTypes[i].head != NULL && zSqlTypes[i].pnew != NULL)
	{
		if(0 == strnicmp_q(connstr, zSqlTypes[i].head, strlen(zSqlTypes[i].head)))
		{
			sqltypeinfo = &zSqlTypes[i];
			return (connstr + strlen(zSqlTypes[i].head));
		}

		i++;
	}

	return NULL;
}
