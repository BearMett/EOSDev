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
    };
  }
  
  
  [[eosio::action]]
  void adduser(name contractcreator,name wantuser) {
    if(checkdeployer(contractcreator) == 0) return;
    require_auth(wantuser);
    puser_index ulist(_code, _code.value);
    auto iterator = ulist.find(wantuser.value);
    if( iterator == ulist.end() )
    {
      ulist.emplace(wantuser, [&]( auto& row ) {
        row.user = wantuser;
      });
    }
  }
  
    
  [[eosio::action]]
  void addadmin(name contractcreator,name wantuser) {
    if(checkdeployer(contractcreator) == 0) return;
    require_auth(contractcreator);
    padmin_index admitlist(_code, _code.value);
    auto iterator = admitlist.find(wantuser.value);
    if( iterator == admitlist.end() )
    {
      admitlist.emplace(wantuser, [&]( auto& row ) {
        row.user = wantuser;
      });
    }
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
    padmin_index pulist(_code, _code.value);
    auto iterator = pulist.find(rqster.value);
    auto iter = rqlist.find(rqster.value);
    if(iter == rqlist.end())
    {
      rqlist.emplace(addnet, [&]( auto& row ){
        row.rqster = rqster;
        row.device = targetdevice;
        row.payload = NULL;
        row.isright = !(iterator == pulist.end());
        row.tmstmp = now();
      });
    }
    else{
      rqlist.modify(iter, addnet, [&]( auto& row ){
        row.rqster = rqster;
        row.device = targetdevice;
        row.payload = NULL;
        row.isright = !(iterator == pulist.end());
        row.tmstmp = now();
      });
    }
  }
private:
	int checkdeployer(name tryer){
    name asd=eosio::contract::_self;
    if(tryer == asd) return 1;
    else return 0;
	  
	}
  struct [[eosio::table]] permituser {
    name user;
    uint64_t primary_key() const { return user.value; }
  };
  typedef eosio::multi_index<"permitlist"_n, permituser> puser_index;
  
  
  struct [[eosio::table]] permitadmin {
    name user;
    uint64_t primary_key() const { return user.value; }
  };
  typedef eosio::multi_index<"admitlist"_n, permitadmin> padmin_index;
  
  struct [[eosio::table]] permittedlist {
    name device;
    uint64_t primary_key() const { return device.value; }
  };
  typedef eosio::multi_index<"attachedlist"_n, permittedlist> atched_index;
  
  struct [[eosio::table]] requestedlist {
	name device;
	name rqster;
    uint64_t payload;
    bool isright;
    int tmstmp;
    uint64_t primary_key() const { return rqster.value; }
  };
  typedef eosio::multi_index<"rqstedlist"_n, requestedlist> rqsted_index;
  
  void send_rquest(name targetdevice) {
    action(
      permission_level{get_self(),"active"_n},
      get_self(),
      "notify"_n,
      std::make_tuple(targetdevice, name{targetdevice}.to_string())
    ).send();
  };
  
  
};

EOSIO_DISPATCH( polman, (attachdevice)(adduser)(addadmin)(removedevice)(rqstdata))
