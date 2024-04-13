#include "RLDifficultyNode.hpp"
#include "../utils.hpp"

RLDifficultyNode* RLDifficultyNode::create(const DifficultyInfo& di)
{
	auto* ret = new RLDifficultyNode();

	if (ret && ret->init(di))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

RLDifficultyNode* RLDifficultyNode::create(GJDifficulty difficulty)
{
	auto* ret = new RLDifficultyNode();

	if (ret && ret->init({ difficulty, RL_FEATURE_STATE::NONE }))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLDifficultyNode::init(const DifficultyInfo& di)
{
	if (!CCNodeRGBA::init()) return false;

	m_difficulty_info = di;

	this->setCascadeColorEnabled(true);
	this->setCascadeOpacityEnabled(true);
	m_will_set_anchor = true;
	this->setAnchorPoint({ .0f, .0f });
	m_will_set_anchor = false;


	m_difficulty_sprite = CCSprite::createWithSpriteFrameName(
		rl::constants::difficulty_to_sprite.at(m_difficulty_info.difficulty).data()
	);
	m_difficulty_sprite->setID("difficulty-sprite");
	this->addChild(m_difficulty_sprite);

	this->setContentSize(m_difficulty_sprite->getContentSize());

	switch (m_difficulty_info.feature_state)
	{
		case RL_FEATURE_STATE::FEATURED:
		case RL_FEATURE_STATE::EPIC:
		case RL_FEATURE_STATE::LEGENDARY:
		case RL_FEATURE_STATE::MYTHIC:
			m_feature_sprite = CCSprite::createWithSpriteFrameName(
				rl::constants::feature_state_to_sprite.at(m_difficulty_info.feature_state).data()
			);
		break;

		default:
			m_feature_sprite = nullptr;
		break;
	}

	if (m_feature_sprite)
	{
		m_feature_sprite->setPosition(m_difficulty_sprite->getPosition());
		m_feature_sprite->setID("feature-sprite");
		this->addChild(m_feature_sprite, -1);
	}

	return true;
}

// basically RLDifficultyNode::init but without the call to CCNodeRGBA::init and some extra checks
void RLDifficultyNode::setDifficulty(const DifficultyInfo& di)
{
	if (m_difficulty_info == di) return;

	if (m_difficulty_info.difficulty != di.difficulty)
	{
		m_difficulty_sprite->removeFromParent();

		m_difficulty_sprite = CCSprite::createWithSpriteFrameName(
			rl::constants::difficulty_to_sprite.at(di.difficulty).data()
		);
		m_difficulty_sprite->setID("difficulty-sprite");
		this->addChild(m_difficulty_sprite);
	}

	if (m_difficulty_info.feature_state != di.feature_state)
	{
		if (m_feature_sprite)
			m_feature_sprite->removeFromParent();

		switch (di.feature_state)
		{
			case RL_FEATURE_STATE::FEATURED:
			case RL_FEATURE_STATE::EPIC:
			case RL_FEATURE_STATE::LEGENDARY:
			case RL_FEATURE_STATE::MYTHIC:
				m_feature_sprite = CCSprite::createWithSpriteFrameName(
					rl::constants::feature_state_to_sprite.at(di.feature_state).data()
				);
			break;

			default:
				m_feature_sprite = nullptr;
			break;
		}

		if (m_feature_sprite)
		{
			m_feature_sprite->setID("feature-sprite");
			this->addChild(m_feature_sprite, -1);
		}
	}

	m_difficulty_info = di;
}

void RLDifficultyNode::setDifficulty(GJDifficulty difficulty)
{
	setDifficulty({ difficulty, RL_FEATURE_STATE::NONE });
}

// replacing bad code with even more bad code :D
void RLDifficultyNode::setAnchorPoint(const CCPoint& anchorPoint)
{
	if (m_will_set_anchor)
		CCNodeRGBA::setAnchorPoint(anchorPoint);
}
