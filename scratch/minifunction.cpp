llvm::Function *make_mini_function(std::string name, llvm::LLVMContext context, std::unique_ptr<llvm::Module> module)

MiniFunction::MiniFunction(std::string name) {
    std::vector<llvm::Type *>Doubles(0, llvm::Type::getDoubleTy(context));
    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(context), Doubles, false);
    this = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, mymodule.get());
}