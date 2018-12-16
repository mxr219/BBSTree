#include<stdio.h>
#include<stdlib.h>

#define OK 1         //基本状态常量定义
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define OVERFLOW -1 
                                          
#define LH 1       //左高右低
#define EH 0       //左右等高
#define RH -1      //左低右高

typedef int Status;
typedef int KeyType;     //定义关键字类型 
typedef struct RcdType{  //定义记录类型 
    KeyType key;
    //...其他指针域  
}RcdType;
typedef struct BBSTNode{     
    RcdType rcd;
    int bf;                             //结点平衡因子
    int key;                            //关键字域，方便显示关键字
    struct BBSTNode * lchild,* rchild; //定义左右孩子指针域 
}BBSTNode,*BBSTree; 

void InitBBStree(BBSTree &T){
    T = NULL;
    return ;
}

BBSTree makeNode(RcdType rcd){       //创建节点操作
    BBSTree p = NULL;
    p = (BBSTree) malloc(sizeof(BBSTNode)) ;
    if(NULL==p) return NULL;
    p->rcd = rcd;
    p->bf = EH;
    p->key = rcd.key;
    p->lchild = NULL;
    p->rchild = NULL;
    return p;
}
 
void R_Rotate(BBSTree &p){       //右旋操作
     BBSTree lc = p->lchild;
     p->lchild = lc->rchild;
     lc->rchild = p;
     p = lc;
}
void L_Rotate(BBSTree &p){    //左旋操作
     BBSTree rc =p->rchild;
     p->rchild = rc->lchild;
     rc->lchild = p;
     p = rc;
}

void LeftBalance(BBSTree &T){     // 实现对二叉树T的左平衡处理
     BBSTree lc,rd;
     lc = T->lchild;
     switch(lc->bf){
         
        case LH : T->bf = lc->bf = EH; R_Rotate(T); break; /*LL型*/
           
        case RH : 
             rd = lc->rchild;                             /*LR型*/
             switch(rd->bf){
                case LH:T->bf = RH; lc->bf = EH; break;
                case EH:T->bf = lc->bf = EH; break;
                case RH:T->bf = EH;lc->bf = LH; break;
             } 
             rd->bf = EH;
             L_Rotate(T->lchild);
             R_Rotate(T);
             break;
        case EH:              //删除操作平衡处理
            T->bf = LH ;
            lc->bf = RH ;
            R_Rotate(T);
     }
}
 

void RightBalance(BBSTree &T)  // 实现对二叉树T的右平衡处理 
{
      BBSTree rd,lc;
      rd = T->rchild;
      switch(rd->bf){
      
         case RH: T->bf = rd->bf = EH;      /*RR型*/
                  L_Rotate(T);
                  break;
         case LH:  lc = rd->lchild;        /*RL型*/
         switch(lc->bf){
            case LH: T->bf = EH; rd->bf = RH;break;            
            case RH: T->bf = LH; rd->bf = EH; break;            
            case EH: T->bf = rd->bf =  EH;    break;
            }
           lc->bf = EH;
           R_Rotate(T->rchild);
           L_Rotate(T);
           break;
         
         case EH:                  //删除操作平衡处理
            T->bf =  RH;
            rd->bf = LH;
            L_Rotate(T);
            break;
      }
}
       
Status InsertAVL(BBSTree &T,RcdType e,Status &taller){ //递归插入节点    
    if(NULL==T){      
       T = makeNode(e);
       if(NULL==T) return OVERFLOW;      
       taller = TRUE;
    } else if(T->rcd.key==e.key){   //存在关键字一样的数据,插入失败
        taller = FALSE;
        return ERROR;
    }else if(T->rcd.key>e.key){
        if(FALSE==InsertAVL( T->lchild,e,taller)){
             return FALSE;
        }
        if(TRUE==taller){
           switch(T->bf){
               case LH : LeftBalance(T); taller = FALSE; break;
               case EH : T->bf = LH;taller = TRUE;  break;
               case RH : T->bf = EH;taller = FALSE; break;
           }   
        }
    }else{
       if(FALSE==InsertAVL( T->rchild,e,taller)){
           return FALSE;
       }
       if(TRUE==taller){
         switch(T->bf){
               case LH : T->bf = EH;taller = FALSE;break;
               case EH : T->bf = RH;taller = TRUE; break;
               case RH : RightBalance(T); taller = FALSE; break;               
           }                
       }
    } 
    return TRUE;
}

Status deleteBBSTree(BBSTree &T,KeyType key,Status &shorter){
   if(NULL==T) return ERROR;
   if(key==T->rcd.key){             //相同时删除操作
        if(NULL==T->lchild&&NULL==T->rchild){
           free(T); // 删除叶子节点时 
           T = NULL;
           shorter = TRUE;
        }else if(NULL==T->lchild){       //左子树为空，右子树不为空
           BBSTree p = T;
           T = T->rchild;
           free(p);
           shorter = TRUE;
       }else if(NULL==T->rchild){        //右子树为空，左子树不为空
           BBSTree p = T;
           T = T->lchild;
           free(p);
           shorter = TRUE;       
       }else{                            //左,右子树不为空
            BBSTree p = T->lchild,ppar = T->lchild ;
              while(NULL!=p->rchild){
                 ppar = p;
                 p = p->rchild;
             }
            RcdType rcd;            
            rcd = p->rcd;                 //先保存最左节点的信息，然后将最左节点删除，最后把根节点替换为原本最左节点的值
            T->rcd = rcd;
            T->key = T->rcd.key;
            if(OK== deleteBBSTree(T->lchild,p->key,shorter)){   //最左节点肯定为右子树不存在的点，直接调用函数删除该点
                if(TRUE==shorter){                                       //对根节点再做一次平衡处理
                  switch(T->bf){
                  case LH:  T->bf = EH;      shorter = TRUE;    break;
                  case EH:  T->bf = RH;      shorter = FALSE;   break;
                  case RH:  RightBalance(T); 
                             if(T->lchild->bf==RH){
                              shorter = ERROR ;
                             }else shorter = OK ;
                            break;
                 } 
                }
              }
          
       }
        return OK;
   }else if(key<T->rcd.key){                                                 //左边删除并处理
             if(ERROR==deleteBBSTree(T->lchild,key,shorter)) return ERROR;
              if(TRUE==shorter){
                  switch(T->bf){
                  case LH:  T->bf = EH;      shorter = TRUE;    break;
                  case EH:  T->bf = RH;      shorter = FALSE;   break;
                  case RH:  RightBalance(T);        
                           if(T->lchild->bf==RH){
                              shorter = ERROR ;
                           }else shorter = OK ;   
                           break;
                 } 
              }
             return OK;
   }else{                                                                //有边删除并处理
       if(ERROR==deleteBBSTree(T->rchild,key,shorter))  return ERROR;
       
       if(TRUE==shorter){
          switch(T->bf){
              case LH: LeftBalance(T); 
                       if(T->rchild->bf==LH){
                              shorter =  FALSE;
                       }else shorter =  TRUE  ;
                       break; 
              case EH: T->bf = LH;     shorter = FALSE;  break;
              case RH: T->bf = EH;     shorter = TRUE;   break;
          }         
       }
        return OK;
    } 
}

Status Search(BBSTree T,KeyType key){             //查找返回是否存在关键字，返回状态
     if(T==NULL) return ERROR;
     if(T->rcd.key==key){
          return OK;
     }else if(T->rcd.key>key){
          return  Search( T->lchild, key);
     }else {
          return  Search( T->rchild, key);
     }
}

BBSTree SearchBBSTree(BBSTree T,KeyType key){    //查找返回是否存在关键字，存在返回指针，不存在返回NULL
     if(T==NULL) return NULL;
     if(T->rcd.key==key){
          return T;
     }else if(T->rcd.key>key){
          return  SearchBBSTree( T->lchild, key);
     }else {
          return  SearchBBSTree( T->rchild, key);
     }
}

void InorderTraverse(BBSTree T){         //中序遍历
     if(NULL==T) return ;   
     InorderTraverse(T->lchild);
     printf("%d ",T->rcd.key);
     InorderTraverse(T->rchild);
}

void PreorderTraverse(BBSTree T){         //先序遍历
     if(NULL==T) return ; 
     printf("%d ",T->rcd.key);
     PreorderTraverse(T->lchild);     
     PreorderTraverse(T->rchild);
}

void PosorderTraverse(BBSTree T){         //后序遍历
     if(NULL==T) return ;     
     PosorderTraverse(T->lchild);
     PosorderTraverse(T->rchild);
     printf("%d ",T->rcd.key);
}

void printBBST(BBSTree T,int lev){  //用凹表显示平衡二叉树
    if(NULL==T) return;
    printBBST( T->rchild, lev+1);
    
    for(int i=0;i<lev;i++){
        printf("     ");
    }
    printf("%d\n",T->rcd.key);
    printBBST( T->lchild, lev+1);
    return;
}

void destroy(BBSTree &T){            //销毁平衡二叉树
     if(T==NULL) return ;
     destroy(T->lchild);
     destroy(T->rchild);
     free(T);
     T = NULL;
}

//统计叶子节点数目
void countLeaf(BBSTree T,int &count){
    if(NULL==T) return ;
    if(NULL==T->lchild&&NULL==T->rchild){
        count++;    
    }
    countLeaf( T->lchild,count);
    countLeaf( T->rchild,count);
}  

//统计二叉树深度
int depth(BBSTree T){
    if(NULL==T) return 0;
    int ldepth = depth(T->lchild);
    int rdepth = depth(T->rchild);
    return ldepth>rdepth? 1+ldepth:1+rdepth ;
}

void split(BBSTree T,KeyType key,BBSTree &T1,BBSTree &T2){
   if(NULL==T) return ;
   Status taller = FALSE;
   if(T->rcd.key>key) {
       InsertAVL(T2,T->rcd,taller);
   }else{
       InsertAVL(T1,T->rcd,taller);
   }
   split( T->lchild, key, T1,T2);
   split( T->rchild, key, T1,T2);
}

void splitBBST(BBSTree T,KeyType key,BBSTree &T1,BBSTree &T2){ //分裂平衡二叉树
     if(T1!=NULL) destroy(T1);
     if(T2!=NULL) destroy(T2);
     T1 = NULL;
     T2 = NULL;
     split( T, key, T1, T2);
}

void mergeBBSTree(BBSTree &T1,BBSTree &T2){  //合并两颗树
     Status taller = FALSE;
     if(NULL==T2) return;
     if(T2->lchild) mergeBBSTree(T1,T2->lchild);
     if(T2->rchild) mergeBBSTree(T1,T2->rchild);
     InsertAVL(T1,T2->rcd,taller);
     free(T2);
     return;
}
void main(){
      BBSTree T,T1,T2,T3,ST;      //T1用于合并,T2,T3用于分裂时存储,ST用于查找时存储特定元素指针
      Status taller  = ERROR;
      Status shorter = ERROR;  
      RcdType e ;
      int avlNum,lev=0; 
      
      printf("请输入您想新建的平衡二叉树的元素个数\n");
      scanf("%d",&avlNum);
      printf("请输入所有的关键字\n");
      
      //新建平衡二叉树
      for(int i=0;i<avlNum;i++) {      
          scanf("%d",e.key);
        InsertAVL(T,e,taller);                           
      }
       printf("=====平衡二叉树成功生成======\n");
       printBBST(T,lev); //显示新建的二叉树
      //各个功能测试
      while(true){
         printf("========功能表==============\n");
         printf(" 1:前序遍历输出平衡二叉树\n");
         printf(" 2:中序遍历输出平衡二叉树\n");
         printf(" 3:后序遍历输出平衡二叉树\n");
         printf(" 4:插入新元素\n");
         printf(" 5:删除特定元素\n");
         printf(" 6:查找并输出元素信息\n");
         printf(" 7:统计叶子节点数目\n");
         printf(" 8:求该平衡二叉树的深度\n");
         printf(" 9:用凹表输出平衡二叉树\n");
         printf("10:将平衡二叉树分裂成两棵子树\n");
         printf("11:将两颗平衡二叉树合并成一棵\n");
         printf("12:销毁平衡二叉树\n");
         printf("13:结束\n");         
         printf("===========================\n");
         printf("请选择: \n");
         int choose = 0;
         scanf("%d",&choose);
         if(choose>13||choose<1){
           printf("您输入指令有误,请重新输入\n");   //判断输入是否错误
            continue;
         }
         switch(choose){
            case 1: printf("前序遍历的结果为:  ");
                    PreorderTraverse(T);
                    printf("\n");
                    break;
            case 2: printf("中序遍历的结果为:  ");
                    InorderTraverse(T);
                    printf("\n");
                    break;
            case 3: printf("后序遍历的结果为:  ");
                    PosorderTraverse(T);
                    printf("\n");
                    break;
            case 4: printf("请输入要插入的新元素\n");
                    scanf("%d",e.key);
                    if(OK==InsertAVL(T,e,taller)){
                       printf("新元素插入成功\n");
                       printf("更新后的二叉树如下: \n");                      
                       printBBST(T,lev);
                    }else{
                       printf("该元素已存在,插入失败\n");
                    }
                    taller = ERROR;
                    break;
            case 5: printf("请输入要删除的关键字\n");
                    scanf("%d",e.key);
                    if(OK==deleteBBSTree(T,e.key,shorter)){
                       printf("元素删除成功\n");
                       printf("更新后的二叉树如下: \n");                      
                       printBBST(T,lev);
                       shorter  = ERROR;
                    }else{
                       printf("平衡二叉树中不存在该元素,删除失败\n");
                    }
                    break;                    
            case 6:printf("请输入要查找的元素\n");
                   scanf("%d",e.key);
                   ST = SearchBBSTree( T,e.key);
                   if( ST!=NULL ){
                   printf("查找成功,元素信息如下\n");
                   printf("关键字: key = %d ,平衡因子bf= %d\n",ST->key,ST->bf);
                  }else{ 
                    printf("该关键字不存在,查找失败\n");                    
                  } break;
           case 7:int count = 0;
                   countLeaf(T,count);
                   printf("该平衡二叉树的叶子节点数目为:%d\n",count);
                   break;
           case 8:printf("该平衡二叉树的深度为: %d\n",depth(T));
                  break;
           case 9:printf("用凹表显示如下\n");
                  printBBST(T,lev);
                  break;
           case 10:printf("请输入分裂划分边界  ");
                   int splitfator ;
                   scanf("%d",&splitfator);
                   splitBBST( T,splitfator , T2, T3);
                   printf("二叉树分裂成功\n");
                   printf("比%d小的值构成的二叉树为: T2 如下所示\n",splitfator);
                   printBBST(T2,lev);
                   printf("比%d大的值构成的二叉树为: T3 如下所示\n",splitfator);
                   printBBST(T3,lev);
                   break; 
           case 11:printf("请输入要新建的平衡二叉树的个数:\n");
                   scanf("%d",&avlNum);
                   printf("请输入所有的关键字\n");
           
                   for(int i=0;i<avlNum;i++) {
                       scanf("%d",e.key);
                       InsertAVL(T1,e,taller);                           
                   }
                   mergeBBSTree( T, T1);
                   printf("合并成功,合并后树为:\n");
                   printBBST(T,lev);
                   break;
           case 12: destroy(T);
                   printf("平衡二叉树已销毁\n");
                   break;
           case 13: printf("程序中止,再见\n");
                   return;
          }
      }
}
