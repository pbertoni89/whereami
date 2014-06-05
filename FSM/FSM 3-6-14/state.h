class State
{
public:
    State(void);
    ~State(void);
    virtual State* exec()=0;
};
