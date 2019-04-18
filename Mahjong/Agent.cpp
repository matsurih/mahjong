﻿#include "Agent.h"

using namespace std;

int RealPlayer::get_self_action(Table * status, std::vector<SelfAction> actions)
{
	status->test_show_all();
	cout << endl; cout << "Select:" << endl;
	int i = 0;

	for (auto action : actions) {
		cout << i << " " << action.to_string() << endl;
		++i;
	}
	int selection;
	cin >> selection;
	return selection;
}

int RealPlayer::get_response_action(Table * status, SelfAction action, Tile* tile, std::vector<ResponseAction> actions)
{
	//status->test_show_all();
	
	cout << endl; 
	cout << "You are player " << i_player << "." << endl;
	cout << "Player " << status->turn;
	switch (action.action) {
	case Action::立直:
		cout << " calls riichi and plays ";
		break;
	case Action::出牌:
		cout << " plays ";
		break;
	case Action::暗杠:
		cout << " 暗杠 ";
		break;
	case Action::加杠:
		cout << " 加杠 ";
		break;
	default:
		throw runtime_error("Unknown action.");
	}
	cout<< tile->to_string() << endl;
	
	cout << "Select:" << endl;
	int i = 0;
	for (auto action : actions) {
		cout << i << " " << action.to_string() << endl;
		++i;
	}
	int selection;
	cin >> selection;
	return selection;
}

#include <random>

int RandomPlayer::get_self_action(Table * status, std::vector<SelfAction> actions)
{
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, actions.size() - 1);
	int dice_roll = distribution(generator);  // generates number in the range 1..6 

	for (int i = 0; i < actions.size(); ++i) {
		if (actions[i].action == Action::自摸) {
			dice_roll = i; break;
		}
		if (actions[i].action == Action::立直) {
			dice_roll = i; break;
		}
		if (actions[i].action == Action::九种九牌) {
			dice_roll = i; break;
		}
	}

	ss << status->to_string();
	ss << endl; ss << "Select:" << endl;
	int i = 0;

	for (auto action : actions) {
		if (i == dice_roll)
			ss << "--> ";
		ss << i << " " << action.to_string() << endl;
		++i;
	}

	return dice_roll;
}

int RandomPlayer::get_response_action(Table * status, SelfAction action, Tile *tile, std::vector<ResponseAction> actions)
{
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, actions.size() - 1);
	int dice_roll = distribution(generator);  // generates number in the range 1..6

	for (int i = 0; i < actions.size(); ++i) {
		if (actions[i].action == Action::荣和) {
			dice_roll = i;
			break;
		}
	} 

	ss << endl;
	ss << "You are player " << i_player << "." << endl;
	ss << "Player " << status->turn;
	switch (action.action) {
	case Action::立直:
		ss << " calls riichi and plays ";
		break;
	case Action::出牌:
		ss << " plays ";
		break;
	case Action::暗杠:
		ss << " 暗杠 ";
		break;
	case Action::加杠:
		ss << " 加杠 ";
		break;
	default:
		throw runtime_error("Unknown action.");
	}
	ss << tile->to_string() << endl;

	ss << "Select:" << endl;
	int i = 0;
	for (auto action : actions) {
		if (i == dice_roll)
			ss << "--> ";
		ss << i << " " << action.to_string() << endl;
		++i;
	}

	return dice_roll;
}
