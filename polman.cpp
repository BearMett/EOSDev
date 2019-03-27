#include"polman.hpp"


public:
  using contract::contract;
  
  polman(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {
  }

  [[eosio::action]]
  void attachdevice(name dvice) {
    require_auth(dvice);
    atched_index plist(_code, _code.value);
    auto iterator = plist.find(dvice.value);
    if( iterator == plist.end() )
    {
      plist.emplace(dvice, [&]( auto& row ) {
       row.device = dvice;
      });
    }
    else {
      std::string changes;
      plist.modify(iterator, dvice, [&]( auto& row ) {
        row.device = dvice;
      });
    }
  }
  
  
  [[eosio::action]]
  void adduser(name addnet,uint64_t listnum,name targetuser) {
    require_auth(addnet);
    if(checkdeployer(addnet) == 0) return;
    puser_index ulist(_code, _code.value);
    auto iterator = ulist.find(listnum);
    uint64_t dispensed;
    if( iterator == ulist.end() )
    {
      //dispensed = dispenseusrnumber();
      ulist.emplace(targetuser, [&]( auto& row ) {
        //row.setnum = dispensed;
        row.user = targetuser;
      });
    }/*
    else {
      std::string changes;
      ulist.modify(iterator, targetuser, [&]( auto& row ) {
        row.user = targetuser;
      });
    }*/
  }

  [[eosio::action]]
  void removedevice(name dvice) {
    require_auth(dvice);

    atched_index plist(_self, _code.value);
    auto iterator = plist.find(dvice.value);
    eosio_assert(iterator != plist.end(), "Record does not exist");
    plist.erase(iterator);
  }
  
  
  [[eosio::action]]
  void rqstdata(name addnet,name rqster,name targetdevice){
    if(checkdeployer(addnet) == 0) return;
    require_auth(rqster);
    rqsted_index rqlist(_self,_code.value);
    //uint64_t dispensed = dispenserqstnumber();
    puser_index pulist(_code, _code.value);
    auto iterator = pulist.find(rqster.value);
    rqlist.emplace(addnet, [&]( auto& row ){
      //row.rqstid = dispensed;
      row.rqster = rqster;
      row.device = targetdevice;
      row.payload = NULL;
      row.isright = !(iterator == pulist.end());
    });
  }
  
private:
  
  /*static int dispenseusrnumber(){
    static uint64_t datacounter=0;
    datacounter = datacounter + 1;
    return datacounter;
  }*/
  /*static int dispenserqstnumber(){
    static uint64_t datacounter=0;
    datacounter = datacounter + 1;
    return datacounter;
  }*/
  
	int checkdeployer(name tryer){
    name asd=eosio::contract::_self;
    if(tryer == asd) return 1;
    else return 0;
	  
	}
  struct [[eosio::table]] permitteduser {
    //uint64_t setnum;
    name user;
    uint64_t primary_key() const { return setnum; }
    uint64_t get_secondary_1() const {return user.value; }
  };
  typedef eosio::multi_index<"permitlist"_n, permitteduser, indexed_by<"byuser"_n, const_mem_fun<permitteduser, uint64_t, &permitteduser::get_secondary_1>>> puser_index;
  
  struct [[eosio::table]] permittedlist {
    name device;
    uint64_t primary_key() const { return device.value; }
  };
  typedef eosio::multi_index<"attachedlist"_n, permittedlist> atched_index;
  
  struct [[eosio::table]] requestedlist {
	//uint64_t rqstid;
	name device;
	name rqster;
    uint64_t payload;
    bool isright;
    uint64_t primary_key() const { return rqstid; }
  };
  typedef eosio::multi_index<"rqstedlist"_n, requestedlist> rqsted_index;
  
  
  
};

EOSIO_DISPATCH( polman, (attachdevice)(adduser)(removedevice)(rqstdata))
