
class PeticionComidaBebida {
	private:
		int id;
		int num_palomitas;
		int num_bebidas;
	public:
		PeticionComidaBebida(int id, int num_palomitas, int num_bebidas);
		int getId();
		int getNumPalomitas();
		int getNumBebidas();
};
PeticionComidaBebida::PeticionComidaBebida(int id, int num_palomitas, int num_bebidas){
	this->id=id;
	this->num_palomitas=num_palomitas;
	this->num_bebidas=num_bebidas;
}

int PeticionComidaBebida::getId(){
	return this->id;
}

int PeticionComidaBebida::getNumPalomitas(){
	return this->num_palomitas;
}
int PeticionComidaBebida::getNumBebidas(){
	return this->num_bebidas;
}