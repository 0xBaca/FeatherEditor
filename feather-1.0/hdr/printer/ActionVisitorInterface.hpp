#ifndef ACTION_VISITOR_INTERFACE
#define ACTION_VISITOR_INTERFACE

namespace feather::windows::action
{
} // namespace feather::windows

namespace feather::printer
{
class ActionVisitorInterface
{
public:
  virtual void visit(windows::action::SearchSubstringAction*) = 0;
};
} // namespace feather::printer

#endif
