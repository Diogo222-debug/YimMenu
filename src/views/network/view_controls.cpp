﻿#include "fiber_pool.hpp"
#include "gta_util.hpp"
#include "util/troll.hpp"
#include "util/toxic.hpp"
#include "views/view.hpp"

#include <network/Network.hpp>
#include <script/globals/GPBD_FM_3.hpp>
#include <script/globals/GlobalPlayerBD.hpp>


namespace big
{
	struct SessionType
	{
		eSessionType id;
		const char* name;
	};

	void render_player_options()
	{
		ImGui::SeparatorText("PLAYER_OPTIONS"_T.data());

		ImGui::BeginGroup();
		components::command_button<"killall">({}, "KILL_ALL"_T);
		components::command_button<"explodeall">({}, "EXPLODE_ALL"_T);
		components::command_button<"beastall">({});
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("INCLUDING_YOU"_T.data());
		components::command_button<"bringall">({});
		components::command_button<"giveweapsall">({});
		components::command_button<"remweapsall">({});
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		components::command_button<"ceokickall">({});
		components::command_button<"vehkickall">({});
		components::command_button<"deletevehall">({});
		components::command_button<"ragdollall">({}, "RAGDOLL_PLAYERS"_T);
		components::command_button<"intkickall">({}, "KICK_ALL_FROM_INTERIORS"_T);
		components::command_button<"missionall">({});
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		components::command_button<"errorall">({});
		components::command_button<"ceoraidall">({});
		components::button("TRIGGER_MC_RAID"_T, [] {
			g_player_service->iterate([](auto& plyr) {
				toxic::start_activity(plyr.second, eActivityType::BikerDefend);
			});
		});
		components::button("TRIGGER_BUNKER_RAID"_T, [] {
			g_player_service->iterate([](auto& plyr) {
				toxic::start_activity(plyr.second, eActivityType::GunrunningDefend);
			});
		});
		ImGui::EndGroup();

		components::small_text("BOUNTY"_T);
		static int value = 10000;
		ImGui::SetNextItemWidth(300);
		ImGui::SliderInt("##bountyvalue", &value, 0, 1000000);
		components::command_checkbox<"anonbounty">();
		components::button("Bounty All", [] {
			g_player_service->iterate([](auto& plyr) {
				troll::set_bounty_on_player(plyr.second, value, g.session.anonymous_bounty);
			});
		});
	}

	void render_session_control()
	{
		ImGui::SeparatorText("SESSION_CONTROL"_T.data());

		ImGui::BeginGroup();
		ImGui::Checkbox("SCRIPT_BLOCK_AMMUNATION"_T.data(), &g.session.script_block_opts.ammunation);
		ImGui::Checkbox("SCRIPT_BLOCK_ATMS"_T.data(), &g.session.script_block_opts.atms);
		ImGui::Checkbox("SCRIPT_BLOCK_ARCADE_GAMES"_T.data(), &g.session.script_block_opts.arcade_games);
		ImGui::Checkbox("SCRIPT_BLOCK_CASINO_GAMES"_T.data(), &g.session.script_block_opts.casino_games);
		ImGui::Checkbox("SCRIPT_BLOCK_DRONES"_T.data(), &g.session.script_block_opts.drones);
		ImGui::Checkbox("SCRIPT_BLOCK_GANG_ATTACKS"_T.data(), &g.session.script_block_opts.gang_attacks);
		ImGui::Checkbox("SCRIPT_BLOCK_IMPROMPTU_DEATHMATCHES"_T.data(), &g.session.script_block_opts.impromptu_dm);
		ImGui::Checkbox("SCRIPT_BLOCK_IMPROMPTU_RACES"_T.data(), &g.session.script_block_opts.impromptu_race);
		ImGui::Checkbox("SCRIPT_BLOCK_INTERIORS"_T.data(), &g.session.script_block_opts.interiors);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("SCRIPT_BLOCK_INTERIORS_DESC"_T.data());
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::Checkbox("SCRIPT_BLOCK_LSC"_T.data(), &g.session.script_block_opts.lsc);
		ImGui::Checkbox("SCRIPT_BLOCK_MOVIES"_T.data(), &g.session.script_block_opts.movies);
		ImGui::Checkbox("SCRIPT_BLOCK_PROSTITUTES"_T.data(), &g.session.script_block_opts.prostitutes);
		ImGui::Checkbox("SCRIPT_BLOCK_SITTING"_T.data(), &g.session.script_block_opts.sitting);
		ImGui::Checkbox("SCRIPT_BLOCK_SLEEPING"_T.data(), &g.session.script_block_opts.sleeping);
		ImGui::Checkbox("SCRIPT_BLOCK_STORES"_T.data(), &g.session.script_block_opts.stores);
		ImGui::Checkbox("SCRIPT_BLOCK_STREET_DEALER"_T.data(), &g.session.script_block_opts.street_dealer);
		ImGui::Checkbox("SCRIPT_BLOCK_STRIP_CLUB"_T.data(), &g.session.script_block_opts.strip_club);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("SCRIPT_BLOCK_STRIP_CLUB_DESC"_T.data());
		ImGui::Checkbox("SCRIPT_BLOCK_VENDING_MACHINES"_T.data(), &g.session.script_block_opts.vending_machines);
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		components::sub_title("REQUIRES_SCRIPT_HOST"_T);
		ImGui::Checkbox("DISABLE_CEO_MONEY"_T.data(), &g.session.block_ceo_money);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("DISABLE_CEO_MONEY_DESC"_T.data());
		ImGui::Checkbox("BLOCK_JOBS"_T.data(), &g.session.block_jobs);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("BLOCK_JOBS_DESC"_T.data());
		ImGui::Checkbox("RANDOMIZE_CEO_COLORS"_T.data(), &g.session.randomize_ceo_colors);
		components::script_patch_checkbox("BLOCK_MUGGERS"_T, &g.session.block_muggers, "BLOCK_MUGGERS_DESC"_T.data());
		components::script_patch_checkbox("BLOCK_CEO_RAIDS"_T, &g.session.block_ceo_raids, "BLOCK_CEO_RAIDS_DESC"_T);
		components::command_checkbox<"blockceos">();
		ImGui::EndGroup();
	}

	void render_force_host_options()
	{
		ImGui::BeginGroup();
		components::small_text("FORCE_HOST"_T);
		
		ImGui::Checkbox("KICK_HOST_TO_STAY_IN_SESSION"_T.data(), &g.session.kick_host_to_stay_in_session);

		ImGui::Checkbox("FORCE_SCRIPT_HOST"_T.data(), &g.session.force_script_host);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("FORCE_SCRIPT_HOST_DESC"_T.data());

		ImGui::SameLine();

		ImGui::Checkbox("FAST_JOIN"_T.data(), &g.session.fast_join);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("FAST_JOIN_DESC"_T.data());
		components::command_button<"emptysession">();

		ImGui::EndGroup();
	}

	void render_session_globals()
	{
		// TODO: what are we doing with this?
		ImGui::BeginGroup();
		components::small_text("WARP_TIME"_T);

		components::button("PLUS_1_MINUTE"_T, [] {
			toxic::warp_time_forward_all(60 * 1000);
		});
		ImGui::SameLine();
		components::button("PLUS_5_MINUTES"_T, [] {
			toxic::warp_time_forward_all(5 * 60 * 1000);
		});
		ImGui::SameLine();
		components::button("PLUS_48_MINUTES"_T, [] {
			toxic::warp_time_forward_all(48 * 60 * 1000);
		});

		components::button("PLUS_96_MINUTES"_T, [] {
			toxic::warp_time_forward_all(96 * 60 * 1000);
		});
		ImGui::SameLine();
		components::button("PLUS_200_MINUTES"_T, [] {
			toxic::warp_time_forward_all(200 * 60 * 1000);
		});
		ImGui::SameLine();
		components::button("STOP_TIME"_T, [] {
			toxic::set_time_all(INT_MAX - 3000);
		});
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("STOP_TIME_DESC"_T.data());
		ImGui::EndGroup();

		ImGui::SameLine();

		static int global_wanted_level = 0;

		ImGui::BeginGroup();
		components::small_text("WANTED_LVL"_T);
		ImGui::SetNextItemWidth(150);
		if (ImGui::SliderInt("##wantedlevel", &global_wanted_level, 0, 5))
		{
			scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::id].RemoteWantedLevelAmount = global_wanted_level;
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("FORCE"_T.data(), &g.session.wanted_level_all))
		{
			scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::id].RemoteWantedLevelPlayer = __rdtsc() + 32;
			scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::id].RemoteWantedLevelAmount = global_wanted_level;
		}
		ImGui::EndGroup();
	}

	void view::network_controls()
	{
		render_player_options();
		render_session_control();
		render_force_host_options();
		render_session_globals();
	}
}
