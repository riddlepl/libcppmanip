#ifndef CPPMANIP_669DE9B8657142C39CC3D13F4EB849E8_HPP
#define CPPMANIP_669DE9B8657142C39CC3D13F4EB849E8_HPP
#include <clang/AST/ASTContext.h>

namespace cppmanip
{
namespace clangutil
{

class TranslationUnitHandler
{
public:
    virtual ~TranslationUnitHandler() { }
    virtual void handleTranslationUnit(clang::ASTContext& ctx) = 0;
};

typedef std::shared_ptr<TranslationUnitHandler> TranslationUnitHandlerPtr;

}
}
#endif // CPPMANIP_669DE9B8657142C39CC3D13F4EB849E8_HPP
