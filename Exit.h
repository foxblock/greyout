#ifndef EXIT_H
#define EXIT_H

#include "BaseUnit.h"

class Exit : public BaseUnit
{
public:
    Exit(Level* newParent);
    virtual ~Exit();

    virtual bool load(list<PARAMETER_TYPE >& params);
    virtual bool processParameter(const PARAMETER_TYPE& param);

    virtual void update();

    virtual void hitUnit(const UnitCollisionEntry& entry);

    bool isExiting;
    bool allExited;

protected:
	enum ExitProp {
		epLink=BaseUnit::ufEOL,
		epEOL
	};

	bool checkAllExited() const;

    vector<BaseUnit*> targets;
    vector<string> targetIDs;

	bool showingLinks;
private:

};


#endif // EXIT_H

