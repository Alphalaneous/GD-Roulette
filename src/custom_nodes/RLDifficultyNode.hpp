#pragma once
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/include/ccTypes.h>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/Enums.hpp>

using namespace geode::prelude;

class RLDifficultyNode : public CCNodeRGBA
{
private:
	struct DifficultyInfo
	{
		GJDifficulty difficulty;
		GJFeatureState feature_state = GJFeatureState::None;

		bool operator==(const DifficultyInfo&) const = default;
	} m_difficulty_info;

	CCSprite* m_difficulty_sprite;
	CCSprite* m_feature_sprite;

	bool m_will_set_anchor;

public:
	static RLDifficultyNode* create(const DifficultyInfo&);
	static RLDifficultyNode* create(GJDifficulty);
	bool init(const DifficultyInfo&);

	void setDifficulty(const DifficultyInfo&);
	void setDifficulty(GJDifficulty);

	void setAnchorPoint(const CCPoint&) override;

	const DifficultyInfo& getDifficultyInfo() const { return m_difficulty_info; };
};
