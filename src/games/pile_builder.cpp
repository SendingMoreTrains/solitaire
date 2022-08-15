class PileBuilder
{
private:
    int type_id;
    PileAcceptFunction accept_function;
    PileEmptyAcceptFunction empty_accept_function;
    PileOrderingFunction ordering_function;
    PilePositioningFunction positioning_function;
    Sprite empty_sprite;
    vec2 pile_pos;

public:
    void reset()
    {
        type_id = 0;
        accept_function = PileAcceptFunctions::Any;
        empty_accept_function = PileEmptyAcceptFunctions::Any;
        ordering_function = PileOrderingFunctions::Any;
        positioning_function = PilePositioningFunctions::OffsetCascade;
        empty_sprite = Sprite();
        pile_pos = vec2 { 0, 0 };
    }

    PileBuilder()
    {
        reset();
    }

    Pile* build()
    {
        return new Pile(type_id, accept_function, empty_accept_function, ordering_function, positioning_function, empty_sprite, pile_pos);
    }

    PileBuilder& set_type_id(int type_id)
    {
        this->type_id = type_id;
        return *this;
    }

    PileBuilder& set_accept_function(PileAcceptFunction paf)
    {
        accept_function = paf;
        return *this;
    }

    PileBuilder& set_empty_accept_function(PileEmptyAcceptFunction peaf)
    {
        empty_accept_function = peaf;
        return *this;
    }

    PileBuilder& set_ordering_function(PileOrderingFunction pof)
    {
        ordering_function = pof;
        return *this;
    }

    PileBuilder& set_positioning_function(PilePositioningFunction ppf)
    {
        positioning_function = ppf;
        return *this;
    }

    PileBuilder& set_empty_sprite(Sprite sprite)
    {
        empty_sprite = sprite;
        return *this;
    }

    PileBuilder& set_pile_pos(vec2 pos)
    {
        pile_pos = pos;
        return *this;
    }

};
