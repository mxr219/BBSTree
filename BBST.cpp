#include<stdio.h>
#include<stdlib.h>

#define OK 1         //����״̬��������
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define OVERFLOW -1 
                                          
#define LH 1       //����ҵ�
#define EH 0       //���ҵȸ�
#define RH -1      //����Ҹ�

typedef int Status;
typedef int KeyType;     //����ؼ������� 
typedef struct RcdType{  //�����¼���� 
    KeyType key;
    //...����ָ����  
}RcdType;
typedef struct BBSTNode{     
    RcdType rcd;
    int bf;                             //���ƽ������
    int key;                            //�ؼ����򣬷�����ʾ�ؼ���
    struct BBSTNode * lchild,* rchild; //�������Һ���ָ���� 
}BBSTNode,*BBSTree; 

void InitBBStree(BBSTree &T){
    T = NULL;
    return ;
}

BBSTree makeNode(RcdType rcd){       //�����ڵ����
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
 
void R_Rotate(BBSTree &p){       //��������
     BBSTree lc = p->lchild;
     p->lchild = lc->rchild;
     lc->rchild = p;
     p = lc;
}
void L_Rotate(BBSTree &p){    //��������
     BBSTree rc =p->rchild;
     p->rchild = rc->lchild;
     rc->lchild = p;
     p = rc;
}

void LeftBalance(BBSTree &T){     // ʵ�ֶԶ�����T����ƽ�⴦��
     BBSTree lc,rd;
     lc = T->lchild;
     switch(lc->bf){
         
        case LH : T->bf = lc->bf = EH; R_Rotate(T); break; /*LL��*/
           
        case RH : 
             rd = lc->rchild;                             /*LR��*/
             switch(rd->bf){
                case LH:T->bf = RH; lc->bf = EH; break;
                case EH:T->bf = lc->bf = EH; break;
                case RH:T->bf = EH;lc->bf = LH; break;
             } 
             rd->bf = EH;
             L_Rotate(T->lchild);
             R_Rotate(T);
             break;
        case EH:              //ɾ������ƽ�⴦��
            T->bf = LH ;
            lc->bf = RH ;
            R_Rotate(T);
     }
}
 

void RightBalance(BBSTree &T)  // ʵ�ֶԶ�����T����ƽ�⴦�� 
{
      BBSTree rd,lc;
      rd = T->rchild;
      switch(rd->bf){
      
         case RH: T->bf = rd->bf = EH;      /*RR��*/
                  L_Rotate(T);
                  break;
         case LH:  lc = rd->lchild;        /*RL��*/
         switch(lc->bf){
            case LH: T->bf = EH; rd->bf = RH;break;            
            case RH: T->bf = LH; rd->bf = EH; break;            
            case EH: T->bf = rd->bf =  EH;    break;
            }
           lc->bf = EH;
           R_Rotate(T->rchild);
           L_Rotate(T);
           break;
         
         case EH:                  //ɾ������ƽ�⴦��
            T->bf =  RH;
            rd->bf = LH;
            L_Rotate(T);
            break;
      }
}
       
Status InsertAVL(BBSTree &T,RcdType e,Status &taller){ //�ݹ����ڵ�    
    if(NULL==T){      
       T = makeNode(e);
       if(NULL==T) return OVERFLOW;      
       taller = TRUE;
    } else if(T->rcd.key==e.key){   //���ڹؼ���һ��������,����ʧ��
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
   if(key==T->rcd.key){             //��ͬʱɾ������
        if(NULL==T->lchild&&NULL==T->rchild){
           free(T); // ɾ��Ҷ�ӽڵ�ʱ 
           T = NULL;
           shorter = TRUE;
        }else if(NULL==T->lchild){       //������Ϊ�գ���������Ϊ��
           BBSTree p = T;
           T = T->rchild;
           free(p);
           shorter = TRUE;
       }else if(NULL==T->rchild){        //������Ϊ�գ���������Ϊ��
           BBSTree p = T;
           T = T->lchild;
           free(p);
           shorter = TRUE;       
       }else{                            //��,��������Ϊ��
            BBSTree p = T->lchild,ppar = T->lchild ;
              while(NULL!=p->rchild){
                 ppar = p;
                 p = p->rchild;
             }
            RcdType rcd;            
            rcd = p->rcd;                 //�ȱ�������ڵ����Ϣ��Ȼ������ڵ�ɾ�������Ѹ��ڵ��滻Ϊԭ������ڵ��ֵ
            T->rcd = rcd;
            T->key = T->rcd.key;
            if(OK== deleteBBSTree(T->lchild,p->key,shorter)){   //����ڵ�϶�Ϊ�����������ڵĵ㣬ֱ�ӵ��ú���ɾ���õ�
                if(TRUE==shorter){                                       //�Ը��ڵ�����һ��ƽ�⴦��
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
   }else if(key<T->rcd.key){                                                 //���ɾ��������
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
   }else{                                                                //�б�ɾ��������
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

Status Search(BBSTree T,KeyType key){             //���ҷ����Ƿ���ڹؼ��֣�����״̬
     if(T==NULL) return ERROR;
     if(T->rcd.key==key){
          return OK;
     }else if(T->rcd.key>key){
          return  Search( T->lchild, key);
     }else {
          return  Search( T->rchild, key);
     }
}

BBSTree SearchBBSTree(BBSTree T,KeyType key){    //���ҷ����Ƿ���ڹؼ��֣����ڷ���ָ�룬�����ڷ���NULL
     if(T==NULL) return NULL;
     if(T->rcd.key==key){
          return T;
     }else if(T->rcd.key>key){
          return  SearchBBSTree( T->lchild, key);
     }else {
          return  SearchBBSTree( T->rchild, key);
     }
}

void InorderTraverse(BBSTree T){         //�������
     if(NULL==T) return ;   
     InorderTraverse(T->lchild);
     printf("%d ",T->rcd.key);
     InorderTraverse(T->rchild);
}

void PreorderTraverse(BBSTree T){         //�������
     if(NULL==T) return ; 
     printf("%d ",T->rcd.key);
     PreorderTraverse(T->lchild);     
     PreorderTraverse(T->rchild);
}

void PosorderTraverse(BBSTree T){         //�������
     if(NULL==T) return ;     
     PosorderTraverse(T->lchild);
     PosorderTraverse(T->rchild);
     printf("%d ",T->rcd.key);
}

void printBBST(BBSTree T,int lev){  //�ð�����ʾƽ�������
    if(NULL==T) return;
    printBBST( T->rchild, lev+1);
    
    for(int i=0;i<lev;i++){
        printf("     ");
    }
    printf("%d\n",T->rcd.key);
    printBBST( T->lchild, lev+1);
    return;
}

void destroy(BBSTree &T){            //����ƽ�������
     if(T==NULL) return ;
     destroy(T->lchild);
     destroy(T->rchild);
     free(T);
     T = NULL;
}

//ͳ��Ҷ�ӽڵ���Ŀ
void countLeaf(BBSTree T,int &count){
    if(NULL==T) return ;
    if(NULL==T->lchild&&NULL==T->rchild){
        count++;    
    }
    countLeaf( T->lchild,count);
    countLeaf( T->rchild,count);
}  

//ͳ�ƶ��������
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

void splitBBST(BBSTree T,KeyType key,BBSTree &T1,BBSTree &T2){ //����ƽ�������
     if(T1!=NULL) destroy(T1);
     if(T2!=NULL) destroy(T2);
     T1 = NULL;
     T2 = NULL;
     split( T, key, T1, T2);
}

void mergeBBSTree(BBSTree &T1,BBSTree &T2){  //�ϲ�������
     Status taller = FALSE;
     if(NULL==T2) return;
     if(T2->lchild) mergeBBSTree(T1,T2->lchild);
     if(T2->rchild) mergeBBSTree(T1,T2->rchild);
     InsertAVL(T1,T2->rcd,taller);
     free(T2);
     return;
}
void main(){
      BBSTree T,T1,T2,T3,ST;      //T1���ںϲ�,T2,T3���ڷ���ʱ�洢,ST���ڲ���ʱ�洢�ض�Ԫ��ָ��
      Status taller  = ERROR;
      Status shorter = ERROR;  
      RcdType e ;
      int avlNum,lev=0; 
      
      printf("�����������½���ƽ���������Ԫ�ظ���\n");
      scanf("%d",&avlNum);
      printf("���������еĹؼ���\n");
      
      //�½�ƽ�������
      for(int i=0;i<avlNum;i++) {      
          scanf("%d",e.key);
        InsertAVL(T,e,taller);                           
      }
       printf("=====ƽ��������ɹ�����======\n");
       printBBST(T,lev); //��ʾ�½��Ķ�����
      //�������ܲ���
      while(true){
         printf("========���ܱ�==============\n");
         printf(" 1:ǰ��������ƽ�������\n");
         printf(" 2:����������ƽ�������\n");
         printf(" 3:����������ƽ�������\n");
         printf(" 4:������Ԫ��\n");
         printf(" 5:ɾ���ض�Ԫ��\n");
         printf(" 6:���Ҳ����Ԫ����Ϣ\n");
         printf(" 7:ͳ��Ҷ�ӽڵ���Ŀ\n");
         printf(" 8:���ƽ������������\n");
         printf(" 9:�ð������ƽ�������\n");
         printf("10:��ƽ����������ѳ���������\n");
         printf("11:������ƽ��������ϲ���һ��\n");
         printf("12:����ƽ�������\n");
         printf("13:����\n");         
         printf("===========================\n");
         printf("��ѡ��: \n");
         int choose = 0;
         scanf("%d",&choose);
         if(choose>13||choose<1){
           printf("������ָ������,����������\n");   //�ж������Ƿ����
            continue;
         }
         switch(choose){
            case 1: printf("ǰ������Ľ��Ϊ:  ");
                    PreorderTraverse(T);
                    printf("\n");
                    break;
            case 2: printf("��������Ľ��Ϊ:  ");
                    InorderTraverse(T);
                    printf("\n");
                    break;
            case 3: printf("��������Ľ��Ϊ:  ");
                    PosorderTraverse(T);
                    printf("\n");
                    break;
            case 4: printf("������Ҫ�������Ԫ��\n");
                    scanf("%d",e.key);
                    if(OK==InsertAVL(T,e,taller)){
                       printf("��Ԫ�ز���ɹ�\n");
                       printf("���º�Ķ���������: \n");                      
                       printBBST(T,lev);
                    }else{
                       printf("��Ԫ���Ѵ���,����ʧ��\n");
                    }
                    taller = ERROR;
                    break;
            case 5: printf("������Ҫɾ���Ĺؼ���\n");
                    scanf("%d",e.key);
                    if(OK==deleteBBSTree(T,e.key,shorter)){
                       printf("Ԫ��ɾ���ɹ�\n");
                       printf("���º�Ķ���������: \n");                      
                       printBBST(T,lev);
                       shorter  = ERROR;
                    }else{
                       printf("ƽ��������в����ڸ�Ԫ��,ɾ��ʧ��\n");
                    }
                    break;                    
            case 6:printf("������Ҫ���ҵ�Ԫ��\n");
                   scanf("%d",e.key);
                   ST = SearchBBSTree( T,e.key);
                   if( ST!=NULL ){
                   printf("���ҳɹ�,Ԫ����Ϣ����\n");
                   printf("�ؼ���: key = %d ,ƽ������bf= %d\n",ST->key,ST->bf);
                  }else{ 
                    printf("�ùؼ��ֲ�����,����ʧ��\n");                    
                  } break;
           case 7:int count = 0;
                   countLeaf(T,count);
                   printf("��ƽ���������Ҷ�ӽڵ���ĿΪ:%d\n",count);
                   break;
           case 8:printf("��ƽ������������Ϊ: %d\n",depth(T));
                  break;
           case 9:printf("�ð�����ʾ����\n");
                  printBBST(T,lev);
                  break;
           case 10:printf("��������ѻ��ֱ߽�  ");
                   int splitfator ;
                   scanf("%d",&splitfator);
                   splitBBST( T,splitfator , T2, T3);
                   printf("���������ѳɹ�\n");
                   printf("��%dС��ֵ���ɵĶ�����Ϊ: T2 ������ʾ\n",splitfator);
                   printBBST(T2,lev);
                   printf("��%d���ֵ���ɵĶ�����Ϊ: T3 ������ʾ\n",splitfator);
                   printBBST(T3,lev);
                   break; 
           case 11:printf("������Ҫ�½���ƽ��������ĸ���:\n");
                   scanf("%d",&avlNum);
                   printf("���������еĹؼ���\n");
           
                   for(int i=0;i<avlNum;i++) {
                       scanf("%d",e.key);
                       InsertAVL(T1,e,taller);                           
                   }
                   mergeBBSTree( T, T1);
                   printf("�ϲ��ɹ�,�ϲ�����Ϊ:\n");
                   printBBST(T,lev);
                   break;
           case 12: destroy(T);
                   printf("ƽ�������������\n");
                   break;
           case 13: printf("������ֹ,�ټ�\n");
                   return;
          }
      }
}
