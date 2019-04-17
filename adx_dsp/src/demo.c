
//公司人员数据结构
message m_person{
    optional int32 id = 1;//人员ID
    optional string name = 2;
    optional string phone = 3;
    optional string company = 4;
    optional string idcard = 5;
    optional int32 duration = 6;                          //验证间隔
    optional bool statu = 7 [default=true];              //激活状态
    optional int32 comID = 8;
    optional string active_time = 9;                     //最近激活时间
    optional string dead_time = 10;                     //下次到期时间
}

//记录多个employee 数据
message m_employee_toc{
    optional bool succ = 1 [default=true];
    optional string reason=2;
    optional int32 companyID = 3;
    repeated m_person persons = 4;
}

//编辑客户端人员信息
message m_editEmployee_toc{
    optional bool succ = 1 [default=true];
    optional string reason = 2;//succ=false时读取错误原因
    optional m_person person = 3;
}

//    if(type==M_EDITEMPLOYEE_TOC)
{
    proto::m_editEmployee_toc  m_editEmployee_toc;
    if(m_editEmployee_toc.ParseFromArray (temp,temp_size))
    {
        if(m_editEmployee_toc.succ ())
        {
            for(int i=0;i<proEmployeeInforList.persons ().size ();i++)
            {
                if(m_editEmployee_toc.person ().id ()==proEmployeeInforList.persons (i).id ())
                {
                    proEmployeeInforList.persons (i).CopyFrom (m_editEmployee_toc.person ()); 
                    //proEmployeeinforList 是一个m_employee_toc 的类。
                    proEmployeeInforList.persons (i).set_phone (m_editEmployee_toc.person ().phone ());

                    break;
                }
            }
        }
        else qDebug()<<"get m_editEmployee_toc false: "<<QString::fromStdString (m_editEmployee_toc.reason ());
    }
    else qDebug()<<"parse m_editEmployee_toc error";
}




