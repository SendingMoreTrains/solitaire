using PilePositioningFunction = void(*)(vec2, std::vector<Card>&);
namespace PilePositioningFunctions
{
    void OffsetCascade(vec2 pile_pos, std::vector<Card>& cards);
    void Aligned(vec2 pile_pos, std::vector<Card>& cards);
}


using PileEmptyAcceptFunction = bool(*)(std::vector<Card>*);
namespace PileEmptyAcceptFunctions
{
    bool Any(std::vector<Card>*);
    bool Aces(std::vector<Card>* incoming_cards);
    bool Kings(std::vector<Card>* incoming_cards);
}


using PileOrderingFunction = bool(*)(Card*, Card*);
namespace PileOrderingFunctions
{
    bool Any(Card*, Card*);
    bool AlternateColor(Card* top_card, Card* bottom_card);
    bool IncrementingSameSuit(Card* top_card, Card* bottom_card);
}


using PileAcceptFunction = bool(*)(std::vector<Card>*, std::vector<Card>*, PileOrderingFunction);
namespace PileAcceptFunctions
{
    bool Any(std::vector<Card>* cards, std::vector<Card>* incoming_cards, PileOrderingFunction);
    bool AnySingle(std::vector<Card>* cards, std::vector<Card>* incoming_cards, PileOrderingFunction);
    bool AlternateColorsDescendingRank(std::vector<Card>* cards, std::vector<Card>* incoming_cards, PileOrderingFunction);
    bool FollowOrdering(std::vector<Card>* cards, std::vector<Card>* incoming_cards, PileOrderingFunction pof);
}


#include "pile_strategies.cpp"
