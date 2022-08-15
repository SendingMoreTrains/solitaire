namespace PilePositioningFunctions
{
    const int CARD_OFFSET = 13;

    void OffsetCascade(vec2 pile_pos, std::vector<Card>& cards)
    {
        if (cards.empty()) { return; }

        for (int i = 0; i < (int)cards.size(); ++i)
        {
            cards[i].set_position(vec2 { pile_pos.x, pile_pos.y + (i * CARD_OFFSET) });
        }
    }

    void Aligned(vec2 pile_pos, std::vector<Card>& cards)
    {
        if (cards.empty()) { return; }

        for (auto card : cards)
        {
            card.set_position(pile_pos);
        }
    }
}


namespace PileEmptyAcceptFunctions
{
    bool Any(std::vector<Card>*) { return true; }

    bool AnySingle(std::vector<Card>* incoming_cards)
    {
        return incoming_cards->size() == 1;
    }

    bool Aces(std::vector<Card>* incoming_cards)
    {
        return incoming_cards->front().rank == 1;
    }

    bool Kings(std::vector<Card>* incoming_cards)
    {
        return incoming_cards->front().rank == 13;
    }
}


namespace PileOrderingFunctions
{
    bool Any(Card*, Card*) { return true; }

    bool AlternateColor(Card* top_card, Card* bottom_card)
    {
        return top_card->get_color() != bottom_card->get_color();
    }

    bool IncrementingSameSuit(Card* top_card, Card* bottom_card)
    {
        return (top_card->suit == bottom_card->suit) && (bottom_card->rank - top_card->rank == 1);
    }
}


namespace PileAcceptFunctions
{
    bool Any(std::vector<Card>* cards, std::vector<Card>* incoming_cards, PileOrderingFunction)
    { return true; }

    bool AnySingle(std::vector<Card>* cards, std::vector<Card>* incoming_cards, PileOrderingFunction)
    {
        return incoming_cards->size() == 1;
    }

    bool AlternateColorsDescendingRank(std::vector<Card>* cards, std::vector<Card>* incoming_cards, PileOrderingFunction)
    {
        return
            cards->back().get_color() != incoming_cards->front().get_color() // alternates color
            && ((cards->back().rank - incoming_cards->front().rank) == 1);    // rank is one higher
    }

    bool FollowOrdering(std::vector<Card>* cards, std::vector<Card>* incoming_cards, PileOrderingFunction pof)
    {
        return pof(&cards->back(), &incoming_cards->front());
    }

    bool FollowOrderingSingle(std::vector<Card>* cards, std::vector<Card>* incoming_cards, PileOrderingFunction pof)
    {
        if (incoming_cards->size() > 1) { return false; }

        return FollowOrdering(cards, incoming_cards, pof);
    }

    bool None(std::vector<Card>*, std::vector<Card>*, PileOrderingFunction)
    {
        return false;
    }
}
