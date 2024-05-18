#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <list>
#include <sstream>
#include "Time.h"
enum EIdEvent//�������������� �������
{
	//�������� �������
	ID1 = 1,//������ ������
	ID2,//������ ��� �� ����
	ID3,//������ �������
	ID4,//������ ����
};
struct TableUserTtime
{
	TableUserTtime(int table_, std::string user_, Time time_) :
		table(table_), user(user_), time(time_) {};
	//����� �����
	int table;
	//��� ������������
	std::string user;
	//����� ������ ������������ �� ����
	Time time;
};
//����� ��������
Time beginWork;
//����� ��������
Time endWork;
//���-�� ������
size_t nCountTable;
//��������� �� ���
int nCostForHour;
//���-�� ��������� ������
size_t nFreeTable;
//������ ������
std::list<std::string> waitingMans;
//����� �����, ��� ���-�� �����
std::map<int, std::pair<Time, int>> tableHour;
//�������� ������������
std::vector<TableUserTtime> userAndTime;
//������ ��� ������
std::list<std::string> listOut;
std::string lineLast;

void handleID1(std::string sName_, Time timeEvent_);
void handleID2(std::string sName_, Time timeEvent_, int numTable_);
void handleID3(std::string sName_, Time timeEvent_);
void handleID4(std::string sName_, Time timeEvent_);

void parser(std::ifstream& file_);

bool findErrorInName(std::string& str_)
{
	//"a..z, 0..9, _, -" ���������� �������
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

	//����� �� �����
	std::cout << beginWork.str() << std::endl;
	for (const auto& line : listOut)
		std::cout << line << std::endl;
	//�������� �������������
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
	//������� ��� ������
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

	//che�k for correct
	Time lastEvent;

	std::string trash;
	std::getline(file, trash);

	while (!file.eof())
	{
		//������ ������ �� �����
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

		//� ��������� ������� ������ �������������� ���
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
			//������ ������
			handleID1(sName, timeEvent);
			break;
		case (ID2):
			//����
			strStream >> sTable;
			if (!isNumber(sTable) || sTable == "0")
				throw std::runtime_error("no table");
			table = std::atoi(sTable.c_str());
			if(table > nCountTable)
				throw std::runtime_error("no table");
			//������ ��� �� ����
			handleID2(sName, timeEvent, table);
			break;
		case (ID3):
			handleID3(sName, timeEvent);
			break;
		case (ID4):
			//������ ����
			handleID4(sName, timeEvent);
			break;
		}
		if (!strStream.eof())
			throw std::runtime_error("unexpected word");
	}
}
void handleID1(std::string sName_, Time timeEvent_)
{
	//���� ������ ��� � ������������ �����, ������������ ������ "YouShallNotPass"

	//���� � ������ ������
	auto iter = std::find(waitingMans.begin(), waitingMans.end(), sName_);
	if (iter != waitingMans.end())
	{
		//������
		listOut.push_back(timeEvent_.str() + "YouShallNotPass");
		return;
	}

	//���� � ������ ������� �� ������
	auto iter2 = std::find_if(userAndTime.begin(), userAndTime.end(), [sName_](auto t) {
		return t.user == sName_;
		});
	if (iter2 != userAndTime.end())
	{
		//������
		listOut.push_back(timeEvent_.str() + " 13 YouShallNotPass");
		return;
	}

	//���� ������ ������ � ��������� ����, ����� "NotOpenYet"
	if (timeEvent_ < beginWork)
	{
		//������
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

	//���� ���� �����
	if (iterTable != userAndTime.end())
	{
		//������ "PlaceIsBusy"
		listOut.push_back(timeEvent_.str() + " 13 PlaceIsBusy");
		return;
	}

	//���� ������������ � �������
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

	//������������ �� ���� � �� ������
	if (iterUser == userAndTime.end())
	{
		//������ "ClientUnknown"
		listOut.push_back(timeEvent_.str() + " 13 ClientUnknown");
		return;
	}

	//������� ������������ ��������
	Time diff = timeEvent_ - iterUser->time;

	//���������� ����
	tableHour[numTable_].first += diff;
	tableHour[numTable_].second += diff.getHour();

	//����������� ����
	userAndTime.erase(iterUser);
	//������ �� ��� ����� ����� ����
	userAndTime.push_back({ numTable_, sName_, timeEvent_ });
}
void handleID3(std::string sName_, Time timeEvent_)
{
	//������ �������

	//���� � ����� ���� ��������� �����
	if (nFreeTable > 0)
	{
		//������������ ������ "ICanWaitNoLonger!".
		listOut.push_back(timeEvent_.str() + " 13 ICanWaitNoLonger!");
		return;
	}

	//� ������� �������� �������� ������, ��� ����� ����� ������
	if (waitingMans.size() > nCountTable)
	{
		//������ ������
		auto iter = std::find(waitingMans.begin(), waitingMans.end(), sName_);
		waitingMans.erase(iter);
		//� ������������ ������� ID 11.
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
	//����� ������ ������, ����, �� ������� �� ����� ������������� � ��� �������� ������ ������
	tableHour[iterUser->table].first += (timeEvent_ - iterUser->time);
	tableHour[iterUser->table].second += (timeEvent_ - iterUser->time).getHour();
	//���� ������� ������
	if (waitingMans.empty())
	{
		userAndTime.erase(iterUser);
		++nFreeTable;
		return;
	}
	iterUser->time = timeEvent_;
	iterUser->user = waitingMans.front();
	waitingMans.pop_front();
	//���������� ID12
	listOut.push_back(timeEvent_.str() + " 12 " + iterUser->user + ' ' + std::to_string(iterUser->table));
}