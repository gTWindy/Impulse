#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <list>
#include <sstream>
#include "Time.h"
enum EIdEvent//идентификаторы события
{
	//Входящие события
	ID1 = 1,//Клиент пришёл
	ID2,//Клиент сел за стол
	ID3,//Клиент ожидает
	ID4,//Клиент ушёл
};
struct TableUserTtime
{
	TableUserTtime(int table_, std::string user_, Time time_) :
		table(table_), user(user_), time(time_) {};
	//номер стола
	int table;
	//имя пользователя
	std::string user;
	//время посдки пользователя за стол
	Time time;
};
//время открытия
Time beginWork;
//время закрытия
Time endWork;
//кол-во столов
size_t nCountTable;
//стоимость за час
int nCostForHour;
//кол-во свободных столов
size_t nFreeTable;
//список ждущих
std::list<std::string> waitingMans;
//номер стола, его кол-во часов
std::map<int, std::pair<Time, int>> tableHour;
//играющие пользователи
std::vector<TableUserTtime> userAndTime;
//список для печати
std::list<std::string> listOut;
std::string lineLast;

void handleID1(std::string sName_, Time timeEvent_);
void handleID2(std::string sName_, Time timeEvent_, int numTable_);
void handleID3(std::string sName_, Time timeEvent_);
void handleID4(std::string sName_, Time timeEvent_);

void parser(std::ifstream& file_);

bool findErrorInName(std::string& str_)
{
	//"a..z, 0..9, _, -" допустимые символы
	for (const auto& symbol : str_)
	{
		if ('a' <= symbol && symbol <= 'z')
			continue;
		if ('0' <= symbol && symbol <= '9')
			continue;
		if (symbol == '_' || symbol == '-')
			continue;
		return true;
	}
	return false;
}
bool isNumber(const std::string& str_)
{
	return !str_.empty() && 
		std::find_if(str_.begin(), str_.end(), [](auto c) { return !std::isdigit(c); }) == str_.end();
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		return -1;
	std::ifstream file(argv[1]);
	if (!file.is_open())
	{
		std::cout << "File " << argv[1] << " not found.";
		return -1;
	}

	try
	{
		parser(file);
	}
	catch (...)
	{
		std::cout << lineLast;
		return -1;
	}

	//дошли до конца
	std::cout << beginWork.str() << std::endl;
	for (const auto& line : listOut)
		std::cout << line << std::endl;
	//выгоняем пользователей
	std::sort(userAndTime.begin(), userAndTime.end(), [](auto f, auto s) {
		return f.user < s.user;
		});
	waitingMans.sort();

	auto ita = userAndTime.begin();
	auto itb = waitingMans.begin();

	while (ita != userAndTime.end() || itb != waitingMans.end())
	{
		if (ita == userAndTime.end())
		{
			std::cout << endWork.str() << " 11 " << (*itb);
			++itb;
			continue;
		}
		else if (itb == waitingMans.end())
		{
			std::cout << endWork.str() << " 11 " << ita->user << std::endl;;
			tableHour[ita->table].first += (endWork - ita->time);
			tableHour[ita->table].second += (endWork - ita->time).getHour();
			++ita;
			continue;
		}
		if (ita->user < (*itb))
		{
			std::cout << endWork.str() << " 11 " << ita->user;
			tableHour[ita->table].first += (endWork - ita->time);
			tableHour[ita->table].second += (endWork - ita->time).getHour();
			++ita;
		}
		else
		{
			std::cout << endWork.str() << " 11 " << (*itb);
			++itb;
		}
		std::cout << std::endl;
	}
	std::cout << endWork.str() << std::endl;
	//подсчет для столов
	for (int i = 1; i <= nCountTable; ++i)
	{
		const auto& table = tableHour[i];
		std::cout << i << ' ' << (table.second * nCostForHour) << ' ' << table.first.str()
			<< std::endl;
	}
		
}
void parser(std::ifstream& file)
{
	file >> lineLast;
	if(!isNumber(lineLast) || lineLast == "0")
		throw std::runtime_error("no tables");
	nFreeTable = nCountTable = std::atoi(lineLast.c_str());

	file >> lineLast;
	beginWork = lineLast;

	file >> lineLast;
	endWork = lineLast;

	file >> lineLast;
	if (!isNumber(lineLast) || lineLast == "0")
		throw std::runtime_error("no tables");
	nCostForHour = std::atoi(lineLast.c_str());

	//cheсk for correct
	Time lastEvent;

	std::string trash;
	std::getline(file, trash);

	while (!file.eof())
	{
		//чтение строки из файла
		std::getline(file, lineLast);

		listOut.push_back(lineLast);

		std::stringstream strStream(lineLast);
		std::string str;
		strStream >> str;
		Time timeEvent;
		timeEvent = str;

		if (timeEvent < lastEvent)
			throw std::runtime_error("N+1 < N");

		lastEvent = timeEvent;

		EIdEvent idEvent;
		strStream >> str;
		idEvent = EIdEvent(std::atoi(str.c_str()));

		//у входящего события должно присутствовать имя
		std::string sName;
		strStream >> sName;
		//check name for correct
		if (findErrorInName(sName))
			throw std::runtime_error("invalid name");
		std::string sTable;
		int table;
		switch (idEvent)
		{
		case (ID1):
			//клиент пришёл
			handleID1(sName, timeEvent);
			break;
		case (ID2):
			//стол
			strStream >> sTable;
			if (!isNumber(sTable) || sTable == "0")
				throw std::runtime_error("no table");
			table = std::atoi(sTable.c_str());
			if(table > nCountTable)
				throw std::runtime_error("no table");
			//Клиент сел за стол
			handleID2(sName, timeEvent, table);
			break;
		case (ID3):
			handleID3(sName, timeEvent);
			break;
		case (ID4):
			//Клиент ушел
			handleID4(sName, timeEvent);
			break;
		}
		if (!strStream.eof())
			throw std::runtime_error("unexpected word");
	}
}
void handleID1(std::string sName_, Time timeEvent_)
{
	//Если клиент уже в компьютерном клубе, генерируется ошибка "YouShallNotPass"

	//если в списке ждущих
	auto iter = std::find(waitingMans.begin(), waitingMans.end(), sName_);
	if (iter != waitingMans.end())
	{
		//ошибка
		listOut.push_back(timeEvent_.str() + "YouShallNotPass");
		return;
	}

	//если в списке сидящих за столом
	auto iter2 = std::find_if(userAndTime.begin(), userAndTime.end(), [sName_](auto t) {
		return t.user == sName_;
		});
	if (iter2 != userAndTime.end())
	{
		//ошибка
		listOut.push_back(timeEvent_.str() + " 13 YouShallNotPass");
		return;
	}

	//Если клиент пришел в нерабочие часы, тогда "NotOpenYet"
	if (timeEvent_ < beginWork)
	{
		//ошибка
		listOut.push_back(timeEvent_.str() + " 13 NotOpenYet");
		return;
	}

	waitingMans.push_back(sName_);

}
void handleID2(std::string sName_, Time timeEvent_, int numTable_)
{
	bool bIsUserInQueu = false;

	auto iter = std::find(waitingMans.begin(), waitingMans.end(), sName_);
	if (iter != waitingMans.end())
		bIsUserInQueu = true;

	auto iterTable = std::find_if(userAndTime.begin(), userAndTime.end(), [numTable_](auto t) {
		return t.table == numTable_;
		});

	//если стол занят
	if (iterTable != userAndTime.end())
	{
		//ошибка "PlaceIsBusy"
		listOut.push_back(timeEvent_.str() + " 13 PlaceIsBusy");
		return;
	}

	//если пользователь в очереди
	if (bIsUserInQueu)
	{
		waitingMans.erase(iter);
		userAndTime.emplace_back(numTable_, sName_, timeEvent_);
		--nFreeTable;
		return;
	}

	auto iterUser = std::find_if(userAndTime.begin(), userAndTime.end(), [sName_](auto t) {
		return t.user == sName_;
		});

	//пользователь не ждет и не играет
	if (iterUser == userAndTime.end())
	{
		//ошибка "ClientUnknown"
		listOut.push_back(timeEvent_.str() + " 13 ClientUnknown");
		return;
	}

	//сколько пользователь просидел
	Time diff = timeEvent_ - iterUser->time;

	//прибавляем часы
	tableHour[numTable_].first += diff;
	tableHour[numTable_].second += diff.getHour();

	//освобождаем стол
	userAndTime.erase(iterUser);
	//теперь за ним сидит новый юзер
	userAndTime.push_back({ numTable_, sName_, timeEvent_ });
}
void handleID3(std::string sName_, Time timeEvent_)
{
	//Клиент ожидает

	//Если в клубе есть свободные столы
	if (nFreeTable > 0)
	{
		//генерируется ошибка "ICanWaitNoLonger!".
		listOut.push_back(timeEvent_.str() + " 13 ICanWaitNoLonger!");
		return;
	}

	//В очереди ожидания клиентов больше, чем общее число столов
	if (waitingMans.size() > nCountTable)
	{
		//клиент уходит
		auto iter = std::find(waitingMans.begin(), waitingMans.end(), sName_);
		waitingMans.erase(iter);
		//и генерируется событие ID 11.
		listOut.push_back(timeEvent_.str() + " 11 " + sName_);
	}
}
void handleID4(std::string sName_, Time timeEvent_)
{
	auto iterUser = std::find_if(userAndTime.begin(), userAndTime.end(), [sName_](auto userTable) {
		return userTable.user == sName_;
		});

	if (iterUser == userAndTime.end())
	{
		listOut.push_back(timeEvent_.str() + " 13 ClientUnknown");
		return;
	}
	//Когда клиент уходит, стол, за которым он сидел освобождается и его занимает первый клиент
	tableHour[iterUser->table].first += (timeEvent_ - iterUser->time);
	tableHour[iterUser->table].second += (timeEvent_ - iterUser->time).getHour();
	//если очередь пустая
	if (waitingMans.empty())
	{
		userAndTime.erase(iterUser);
		++nFreeTable;
		return;
	}
	iterUser->time = timeEvent_;
	iterUser->user = waitingMans.front();
	waitingMans.pop_front();
	//генерируем ID12
	listOut.push_back(timeEvent_.str() + " 12 " + iterUser->user + ' ' + std::to_string(iterUser->table));
}