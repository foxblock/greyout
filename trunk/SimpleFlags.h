#ifndef SIMPLEFLAGS_H
#define SIMPLEFLAGS_H

/**
Combining several binary values in one for clarity
**/

class SimpleFlags
{
public:
    SimpleFlags() {flags = 0;}
    ~SimpleFlags() {}

    int flags;

    void clear() {flags = 0;}

    /// All of the following functions also work with multiple (combined) flags passed as "var"
    // Checks whether the passed flag is in flags
    bool hasFlag(const int &var) const
    {
        if (var <= 0)
            return false;
        return ((flags & var) == var);
    };

    // Adds a flag to flags
    void addFlag(const int &var)
    {
        if (var <= 0)
            return;
        flags = (flags | var);
    };

    // Removes a flag from var (making sure it was actually present)
    void removeFlag(const int &var)
    {
        if (var <= 0)
            return;
        flags = (flags ^ (flags & var));
    };
};

#endif // SIMPLEFLAGS_H
