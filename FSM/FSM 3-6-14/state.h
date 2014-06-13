class State
{
public:
    State(void);
    ~State(void);
    virtual State* executeState()=0;
};
