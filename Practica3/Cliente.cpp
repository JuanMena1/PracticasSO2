
class Cliente {
	private:
		int id;
		int num_tickets;
		int num_palomitas;
		int num_bebidas;
	public:
		Cliente(int id, int num_tickets, int num_palomitas, int num_bebidas);
		void comprarTickets();
		void comprarPalomitas();
		int getId();
};

Cliente::Cliente(int id, int num_tickets, int num_palomitas, int num_bebidas){
	this->id=id;
	this->num_tickets=num_tickets;
	this->num_palomitas=num_palomitas;
	this->num_bebidas=num_bebidas;
}

int Cliente::getId(){
	return this->id;
}

