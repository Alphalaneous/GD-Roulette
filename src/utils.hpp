#pragma once
#include <string>
#include <random>
#include <vector>
#include <map>
#include <matjson.hpp>
#include <rtrp/objects/LevelObject.hpp>
#include <rtrp/objects/CreatorObject.hpp>

#include <Geode/binding/GJGameLevel.hpp>

#include "custom_layers/base/BaseCustomAlertLayer.hpp"
#include "listfetcher/ListFetcher.hpp"

namespace rl
{
	namespace utils
	{
		inline int randomInt(int min, int max)
		{
			static std::random_device device;
			static std::mt19937 generator(device());
			std::uniform_int_distribution<int> distribution(min, max);

			return distribution(generator);
		}

		template<typename T>
		inline std::ptrdiff_t getIndexOf(const std::vector<matjson::Value>& vec, T to_find)
		{
			auto it = std::find_if(vec.cbegin(), vec.cend(), [&](const matjson::Value& value) {
				return value.as<T>() == to_find;
			});

			return it != vec.cend() ? (it - vec.cbegin()) : -1;
		}

		template<typename T>
		inline std::ptrdiff_t getIndexOf(const std::vector<T>& vec, T to_find)
		{
			auto it = std::find_if(vec.cbegin(), vec.cend(), [&](const T& value) {
				return value == to_find;
			});

			return it != vec.cend() ? (it - vec.cbegin()) : -1;
		}

		template<typename T>
		inline std::size_t getCountOf(const std::vector<matjson::Value>& vec, T to_find)
		{
			return std::count_if(vec.cbegin(), vec.cend(), [&](const matjson::Value arr) {
				return arr.as<T>() == to_find;
			});
		}

		inline GJGameLevel* createLevelFromResponse(const ListFetcher::level_pair_t& lp)
		{
			auto level = GJGameLevel::create();

			level->m_levelID = lp.first.levelID;
			level->m_levelName = lp.first.name;
			level->m_levelDesc = lp.first.description;
			level->m_levelVersion = lp.first.version;
			level->m_creatorName = lp.second.name;
			level->m_accountID = lp.second.accountID;
			level->m_userID = lp.second.userID;
			level->m_ratings = lp.first.difficultyDenominator;
			level->m_ratingsSum = lp.first.difficultyNumerator;
			level->m_stars = lp.first.stars;
			level->m_coins = lp.first.coins;
			level->m_downloads = lp.first.downloads;
			level->m_likes = lp.first.likes;
			level->m_levelLength = lp.first.length;
			level->m_coinsVerified = lp.first.verifiedCoins;
			level->m_starsRequested = lp.first.starsRequested;
			level->m_featured = lp.first.featureIdx;
			level->m_isEpic = lp.first.epic;

			if (lp.first.isDemon)
			{
				level->m_demon = 1;
				level->m_difficulty = static_cast<GJDifficulty>(lp.first.demonDifficulty + 4);
				level->m_demonDifficulty = lp.first.demonDifficulty;
			}
			else if (lp.first.isAuto)
				level->m_difficulty = GJDifficulty::Auto;
			else
				level->m_difficulty = static_cast<GJDifficulty>(lp.first.difficultyNumerator / lp.first.difficultyDenominator);

			if (auto id = lp.first.customSongID; id != 0)
				level->m_songID = id;
			else
				level->m_audioTrack = lp.first.officialSong;

			level->m_levelNotDownloaded = true;
			level->m_levelType = GJLevelType::Saved;

			return level;
		}

		inline const rtrp::objects::CreatorObject getCreatorFromLevelResponse(
			const std::vector<rtrp::objects::CreatorObject>& creators,
			const rtrp::objects::LevelObject& level
		) {
			auto it = std::find_if(creators.cbegin(), creators.cend(), [&](const auto& creator) {
				return creator.userID == level.creatorUserID;
			});

			return it != creators.cend() ? creators[it - creators.cbegin()] : rtrp::objects::CreatorObject{ level.creatorUserID, "-", 0 };
		}

		inline GJDifficulty getDifficultyFromResponse(const rtrp::objects::LevelObject& level)
		{
			if (level.isAuto)
				return GJDifficulty::Auto;

			if (level.isDemon)
			{
				switch (level.demonDifficulty)
				{
				case 3: return GJDifficulty::DemonEasy;
				case 4: return GJDifficulty::DemonMedium;
				case 5: return GJDifficulty::DemonInsane;
				case 6: return GJDifficulty::DemonExtreme;
				default:
				case 0: return GJDifficulty::Demon;
				}
			}

			if (level.difficultyDenominator == 0)
				return static_cast<GJDifficulty>(-1);

			return static_cast<GJDifficulty>(level.difficultyNumerator / level.difficultyDenominator);
		}

		inline CCLabelBMFont* createTextLabel(
			const char* text,
			const CCPoint& position,
			const float scale,
			CCNode* menu,
			const char* font = "bigFont.fnt"
		) {
			CCLabelBMFont* bmFont = CCLabelBMFont::create(text, font);
			bmFont->setPosition(position);
			bmFont->setScale(scale);
			menu->addChild(bmFont);

			return bmFont;
		}

		inline CCMenuItemSpriteExtra* createButton(
			BaseCustomAlertLayer* self,
			const char* texture,
			const CCPoint& position,
			SEL_MenuHandler callback,
			int tag = -1,
			float textureScale = 1.f,
			float sizeMult = 1.2f
		) {
			auto buttonSprite = CCSprite::createWithSpriteFrameName(texture);
			buttonSprite->setScale(textureScale);
			auto button = CCMenuItemSpriteExtra::create(
				buttonSprite,
				self,
				callback
			);
			button->setPosition(position);
			button->setSizeMult(sizeMult);
			if (tag != -1)
				button->setTag(tag);
			self->addChild(button);

			return button;
		}

		template<auto V>
		struct ScopedVar
		{
		private:
			decltype(V) m_dtor_val;

		public:
			ScopedVar(const decltype(V)& ctor_val, const decltype(V)& dtor_val)
				: m_dtor_val(dtor_val)
			{ V = ctor_val; }

			~ScopedVar() { V = m_dtor_val; }
		};
	}

	namespace constants
	{
		inline const std::map<GJDifficulty, std::string_view> difficulty_to_sprite{
			{ static_cast<GJDifficulty>(-2), "difficulty_06_btn_001.png" },
			{ static_cast<GJDifficulty>(-1), "difficulty_00_btn_001.png" },
			{ GJDifficulty::Auto, "difficulty_auto_btn_001.png" },
			{ GJDifficulty::Easy, "difficulty_01_btn_001.png" },
			{ GJDifficulty::Normal, "difficulty_02_btn_001.png" },
			{ GJDifficulty::Hard, "difficulty_03_btn_001.png" },
			{ GJDifficulty::Harder, "difficulty_04_btn_001.png" },
			{ GJDifficulty::Insane, "difficulty_05_btn_001.png" },
			{ GJDifficulty::Demon, "difficulty_06_btn2_001.png" },
			{ GJDifficulty::DemonEasy, "difficulty_07_btn2_001.png" },
			{ GJDifficulty::DemonMedium, "difficulty_08_btn2_001.png" },
			{ GJDifficulty::DemonInsane, "difficulty_09_btn2_001.png" },
			{ GJDifficulty::DemonExtreme, "difficulty_10_btn2_001.png" }
		};

		inline const std::array<std::array<float, 3>, 3> coins_x_pos{
			std::array<float, 3>{ .0f, -110.f, .0f },
			std::array<float, 3>{ -115.f, -105.f, .0f },
			std::array<float, 3>{ -120.f, -110.f, -100.f }
		};

		inline const std::array<GJDifficulty, 6> idx_to_diff{
			GJDifficulty::Easy,
			GJDifficulty::Normal,
			GJDifficulty::Hard,
			GJDifficulty::Harder,
			GJDifficulty::Insane,
			GJDifficulty::Demon
		};

		inline const std::map<GJDifficulty, int> diff_to_idx{
			{ GJDifficulty::Easy, 0 },
			{ GJDifficulty::Normal, 1 },
			{ GJDifficulty::Hard, 2 },
			{ GJDifficulty::Harder, 3 },
			{ GJDifficulty::Insane, 4 },
			{ GJDifficulty::Demon, 5 }
		};

		inline const std::array<GJDifficulty, 5> idx_to_demon_diff{
			GJDifficulty::DemonEasy,
			GJDifficulty::DemonMedium,
			GJDifficulty::Demon,
			GJDifficulty::DemonInsane,
			GJDifficulty::DemonExtreme
		};

		inline const std::map<GJDifficulty, int> demon_diff_to_idx{
			{ GJDifficulty::DemonEasy, 0 },
			{ GJDifficulty::DemonMedium, 1 },
			{ GJDifficulty::Demon, 2 },
			{ GJDifficulty::DemonInsane, 3 },
			{ GJDifficulty::DemonExtreme, 4 }
		};
	}
}
