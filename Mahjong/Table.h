﻿#ifndef TABLE_H
#define TABLE_H

#include "Tile.h"
#include "Action.h"
#include "GameLog.h"
#include "GameResult.h"
#include "macro.h"
#include "Player.h"
#include <array>
#include <mutex>

constexpr auto N_TILES = (34*4);
constexpr auto INIT_SCORE = 25000;

// Forward Decl
class Table;

class RiverTile {
public:
	Tile* tile;

	// 第几张牌丢下去的
	int number;

	// 是不是立直后弃牌
	bool riichi;

	// 这张牌明面上还在不在河里
	bool remain;

	// true为手切，false为摸切
	bool fromhand;
};

class River {
	// 记录所有的弃牌，而不是仅仅在河里的牌
public:
	std::vector<RiverTile> river;
	inline std::vector<BaseTile> to_basetile() {
		std::vector<BaseTile> basetiles;
		for (auto &tile : river) {
			basetiles.push_back(tile.tile->tile);
		}
		return basetiles;
	}
	inline std::string to_string() const {
		std::stringstream ss;

		for (auto tile : river) {
			ss << tile.tile->to_string() << tile.number;
			if (tile.fromhand)
				ss << "h";
			if (tile.riichi)
				ss << "r";
			if (!tile.remain)
				ss << "-";
			ss << " ";
		}
		return ss.str();
	}

	inline RiverTile& operator[](size_t n) {
		return river[n];
	}

	inline size_t size() {
		return river.size();
	}

	inline void push_back(RiverTile rt) {
		river.push_back(rt);
	}
};

// Forward Decl
class Agent;

class Table
{
private:
	int river_counter = 0;
	Tile tiles[N_TILES];
public:
	Agent* agents[4];	
	int dora_spec; // 翻开了几张宝牌指示牌
	std::vector<Tile*> 宝牌指示牌;
	std::vector<Tile*> 里宝牌指示牌;
	std::vector<Tile*> 牌山;
	// Player players[4];
	std::array<Player, 4> players;
	int turn;
	Action last_action = Action::出牌;
	Wind 场风 = Wind::East;
	int 庄家 = 0; // 庄家
	int n本场 = 0;
	int n立直棒 = 0;

	std::string export_yama();

	inline std::vector<BaseTile> get_dora() const {
		std::vector<BaseTile> doratiles;
		for (int i = 0; i < dora_spec; ++i) {
			doratiles.push_back(get_dora_next(宝牌指示牌[i]->tile));
		}
		return doratiles;
	}

	inline std::vector<BaseTile> get_ura_dora() const {
		std::vector<BaseTile> doratiles;
		for (int i = 0; i < dora_spec; ++i) {
			doratiles.push_back(get_dora_next(里宝牌指示牌[i]->tile));
		}
		return doratiles;
	}

	inline int get_remain_kan_tile() const {
		auto iter = find(牌山.begin(), 牌山.end(), 宝牌指示牌[0]);
		return int(iter - 牌山.begin() - 1);
	}

	inline int get_remain_tile() const {
		return int(牌山.size() - 14);
	}

	void init_tiles();
	void init_red_dora_3();
	void shuffle_tiles();
	void init_yama();

	// std::string export_yama();
	// void import_yama(std::string);
	void init_wind();
	
	void _deal(int i_player);
	void _deal(int i_player, int n_tiles);

	void 发牌(int i_player);
	void 发岭上牌(int i_player);

	inline void next_turn() { turn++; turn = turn % 4; }

	GameLog openGameLog;
	GameLog fullGameLog;

	enum ToStringOption : int {
		YAMA = 1 << 0,
		PLAYER = 1 << 1,
		DORA = 1 << 2,
		N_立直棒 = 1 << 3,
		N_本场 = 1 << 4,
		亲家 = 1 << 5,
		REMAIN_TILE = 1 << 6,
	};

	std::string to_string(int option =
		YAMA | PLAYER | DORA | N_立直棒 | N_本场 | 亲家 | REMAIN_TILE) const;

	inline bool after_chipon(){
		return last_action == Action::吃 ||
			last_action == Action::碰;
	}

	inline bool after_daiminkan() {
		return last_action == Action::杠;
	}

	inline bool after_ankan() {
		return last_action == Action::暗杠;
	}

	inline bool after_加杠() {
		return last_action == Action::加杠;
	}

	inline bool after_杠() {
		return after_daiminkan() || after_加杠();
	}

	Table() = default;
	Table(int 庄家, Agent* p1 = nullptr, Agent* p2 = nullptr, Agent* p3 = nullptr, Agent* p4 = nullptr);
	Table(int 庄家, Agent* p1, Agent* p2, Agent* p3, Agent* p4, int scores[4]);

	// 因为一定是turn所在的player行动，所以不需要输入playerID
	std::vector<SelfAction> GetValidActions();

	std::vector<SelfAction> GetRiichiActions();

	// 根据turn打出的tile，可以做出的决定
	std::vector<ResponseAction> GetValidResponse(int player, Tile* tile, bool);

	// 根据turn打出的tile，可以做出的抢杠决定
	std::vector<ResponseAction> Get抢暗杠(int player, Tile* tile);

	// 根据turn打出的tile，可以做出的抢杠决定
	std::vector<ResponseAction> Get抢杠(int player, Tile* tile);

	void test_show_yama_with_王牌();
	void test_show_yama();
	void test_show_player_hand(int i_player);
	void test_show_all_player_hand();
	void test_show_player_info(int i_player);
	void test_show_all_player_info();
	void test_show_open_gamelog();
	void test_show_full_gamelog();

	inline void test_show_all() {
		test_show_yama_with_王牌();
		test_show_all_player_info();
		//test_show_open_gamelog();
		//test_show_full_gamelog();
		std::cout << "轮到Player" << turn << std::endl;
	}

	Result GameProcess(bool, std::string = "");

	// The following part is for the manual instead of the automatic.
	// Never mix using GameProcess and using the following functions. 
public:
	enum PhaseEnum {
		P1_ACTION, P2_ACTION, P3_ACTION, P4_ACTION,
		P1_RESPONSE, P2_RESPONSE, P3_RESPONSE, P4_RESPONSE,
		P1_抢杠RESPONSE, P2_抢杠RESPONSE, P3_抢杠RESPONSE, P4_抢杠RESPONSE,
		P1_抢暗杠RESPONSE, P2_抢暗杠RESPONSE, P3_抢暗杠RESPONSE, P4_抢暗杠RESPONSE,
		GAME_OVER,
	};

private:
	std::vector<SelfAction> self_action;
	std::vector<ResponseAction> response_action;

	Result result;
	PhaseEnum phase;
	int selection;
	SelfAction selected_action;
	Tile* tile;
	std::vector<ResponseAction> actions; // player actions
	bool FROM_手切摸切;
	Action final_action = Action::pass;
	void _from_beginning();
public:

	// Initialize the game.
	void game_init();

	void game_init_with_metadata(
		std::unordered_map<std::string, std::string> metadata);
	
	// Get the phase of the game
	inline int get_phase() { return (int)phase; }

	// Make a selection and game moves on.
	void make_selection(int selection);

	// Get Information. Return the table itself.
	inline Table* get_info() { return this; }

	// When a player needs response, it can refer to the selection made by the "self-action" player
	inline SelfAction get_full_selected_action() { return selected_action; }
	inline Action get_selected_action() { return selected_action.action; }	
	inline Tile* get_selected_action_tile() { return tile; };

	// Tell that who has to make the selection.
	inline int who_make_selection() { return (get_phase() - Table::P1_ACTION) % 4; }
	
	inline Result get_result() { return result; }	
	inline std::vector<SelfAction> get_self_actions() { return self_action; }
	inline std::vector<ResponseAction> get_response_actions() { return response_action; }

};

#endif