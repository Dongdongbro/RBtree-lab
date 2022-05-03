#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  nil_node->color = RBTREE_BLACK;
  p->root = nil_node;
  p->nil = nil_node;
  return p;
}

void delete_node(rbtree *t, node_t *p){
  if(p!=t->nil){
    delete_node(t,p->left);
    delete_node(t,p->right);
    free(p);
    p = NULL;
  }
}

void delete_rbtree(rbtree *t){
  delete_node(t, t->root);
  free(t->nil);
  t->nil = NULL;
  free(t);
  t = NULL;
}

void left_rotate(rbtree *t, node_t *x){
  node_t *y = x->right; //x노드의  오른쪽에 y노드 지정
  x->right = y->left; //y노드의 왼쪽노드를 x노드의 오른쪽으로 옮긴다.
  //y노드의 왼쪽노드에 노드가 존재한다면, 그 노드의 부모 값을 x로 지정한다.
  if(y->left != t->nil){
    y->left->parent = x;
  }
  //y의 부모값을 x의 부모값으로 지정하여 원래 x가 있던자리에 y를 넣어준다.
  y->parent = x->parent;
  //만약 x가 트리의 루트였다면?
  if(x->parent == t->nil){
    t->root = y; //트리의 루트를 y로 지정해준다.
  }else if(x == x->parent->left){ //x가 왼쪽노드였다면
    x->parent->left = y; //x 부모의 왼쪽노드를 y라고 해준다.
  }else{ // x가 오른쪽 노드였다면,
    x->parent->right = y; // y 부모의 오른쪽 노드를 x라고 해준다.
  }
  y->left = x; //y의 왼쪽노드를 x로 해준다.
  x->parent = y; //x의 부모를 y로 설정해준다.
}

void right_rotate(rbtree *t, node_t *y){
  node_t *x = y->left;
  y->left = x->right;
  if(x->right != t->nil){
    x->right->parent = y;
  }
  x->parent = y->parent;
  if(y->parent == t->nil){
    t->root = x;
  }else if(y==y->parent->right){
    y->parent->right = x;
  }else{
    y->parent->left = x;
  }
  x->right = y;
  y->parent = x;
}

void rbtree_insert_fixup(rbtree *t, node_t *z){
  while (z->parent->color==RBTREE_RED){
    if(z->parent == z->parent->parent->left){
      node_t *y = z->parent->parent->right;
      if (y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }else{
        if(z==z->parent->right){
          z = z->parent;
          left_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);
      }
    }
    else{
      node_t *y = z->parent->parent->left;
      if (y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }else{
        if(z==z->parent->left){
          z = z->parent;
          right_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t,z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 트리의 nil노드에 해당하는 y노드를 생성
  // y를 nil값으로 하는 이유는 밑에 나올 while문이 애초에 돌아가지 않았을 경우 y값이 nil값으로 고정시켜놓기 위해서이다.
  // while문이 애초에 돌아가지 않을 경우는 z노드가 최초 삽입될 경우이다.
  node_t *y = t->nil;
  // 트리의 루트에 해당하는 x노드를 생성
  node_t *x = t->root;
  // 함수파라미터에서 받은 key값을 가진 노드 z를 만든다.
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;
  // x값이 nil노드가 아닐 때 까지 아래로 탐색하는 while문
  while(x!=t->nil){
    // while 문이 마지막 즉, x가 nil을 만나기 전값을 y가 기록하기 위함이다.
    y = x;
    // 만약 z의 key값이 x의 key값보다 작다면
    if(z->key < x->key) {
      // 루트의 왼쪽으로 보내준다.
      x = x->left;
    }else{
      // 루트의 오른쪽으로 보내준다.
      x = x->right;
    }
  }
  // while문에서 저장된 y값을 z-parent에 저장한다.
  z->parent = y;
  // 만약 while문이 한번도 돌지 않았다면,
  if(y==t->nil){
    // 트리의 루트는 z이다
    t->root = z;
  }else if(z->key < y->key){ // z-key, y-key를 비교하여 부모의 노드 구조체에 z 주소값을 넣어주기 위함이다.
    y->left = z;
  }else{
    y->right = z;
  }
  // 트리의 특성상 insert 된 노드는 트리 가장 아래에 위치하게 될 것이기에
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  // 트리의 속성을 위반하는지 판단하고 고쳐주기 위한 함수 호출!
  rbtree_insert_fixup(t, z);
  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *p = t->root;
  while(p!=t->nil && p->key != key){
    if(p->key > key)
    {
      p = p->left;
    }
    else{
      p = p->right;
    }
  }
  if(p->key == key){
    return p;
    }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *p = t->root;
  while(p->left != t->nil){
    p = p->left;
  }
  return p;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *p = t->root;
  while(p->right != t->nil){
    p = p->right;
  }
  return p;
}

void transplant(rbtree *t, node_t *u, node_t *v){
  if(u->parent == t->nil){
    t->root = v;
  }else if(u == u->parent->left){
    u->parent->left = v;
  }else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}

node_t *tree_min(rbtree *t, node_t *p){
  while(p->left != t->nil){
    p = p->left;
  }
  return p;
}

void rbtree_erase_fixup(rbtree *t, node_t *x){
  node_t *u;
  // fix가 시작되는 조건
  while(x != t->root && x->color == RBTREE_BLACK){
    //기준이되는 노드가 왼쪽일 때
    if(x->parent->left == x){
      u = x->parent->right;
      //case1: uncle is red
      if(u->color == RBTREE_RED){
        //형제를 검은색으로 부모를 빨간색으로 칠한다. 부모노드를 기준으로 좌회전한다.
        u->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        u = x->parent->right;
      }
      //case2: uncle is black with two black child
      if(u->left->color == RBTREE_BLACK && u->right->color == RBTREE_BLACK){
        //형제노드만 빨간색으로 만들고 타겟을 부모로 변경한다.
        u->color = RBTREE_RED;
        x = x->parent;
      }
      else { // 자식 중 최소 한개는 빨간색이다.
        //case3: uncle is black with red color left child and black color right child
        if(u->right->color == RBTREE_BLACK){
        // 형제 노드를 빨간색으로, 형제 노드의 왼쪽 자식을 검은색으로 칠하고 형제노드를 기준으로 우회전한다.
          u->color = RBTREE_RED;
          u->left->color = RBTREE_BLACK;
          right_rotate(t, u);
          u = x->parent->right;
        }
        //case4: uncle is black with red color right child
        // 부모 노드의 색을 형제에게 넘긴다. 부모노드와 형제 노드의 오른쪽 자식을 검은색으로 칠한다. 부모 노드 기준으로 좌회전한다.
        u->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        u->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    //기준이 되는 노드가 오른쪽 일때
    else{
      u = x->parent->left;
      //case1: uncle is red
      if(u->color == RBTREE_RED){
        //형제를 검은색으로 부모를 빨간색으로 칠한다. 부모노드를 기준으로 좌회전한다.
        u->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        u = x->parent->left;
      }
      //case2: uncle is black with two black child
      if(u->right->color == RBTREE_BLACK && u->left->color == RBTREE_BLACK){
        //형제노드만 빨간색으로 만들고 타겟을 부모로 변경한다.
        u->color = RBTREE_RED;
        x = x->parent;
      }
      else { // 자식 중 최소 한개는 빨간색이다.
        //case3: uncle is black with red color right child and black color left child
        if(u->left->color == RBTREE_BLACK){
        // 형제 노드를 빨간색으로, 형제 노드의 왼쪽 자식을 검은색으로 칠하고 형제노드를 기준으로 우회전한다.
          u->color = RBTREE_RED;
          u->right->color = RBTREE_BLACK;
          left_rotate(t, u);
          u = x->parent->left;
        }
        //case4: uncle is black with red color left child
        // 부모 노드의 색을 형제에게 넘긴다. 부모노드와 형제 노드의 오른쪽 자식을 검은색으로 칠한다. 부모 노드 기준으로 좌회전한다.
        u->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        u->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}


int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t *y = p;
  color_t y_original_color = y->color;
  node_t *x;
  if(p->left == t->nil){
    x = p->right;
    transplant(t,p,p->right);
  }else if(p->right == t->nil){
    x= p->left;
    transplant(t,p,p->left);
  }else{
    y = tree_min(t, p->right);
    y_original_color = y->color;
    x = y->right;
    if(y->parent==p){
      x->parent=y;
    }else{
      transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    transplant(t,p,y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if(y_original_color == RBTREE_BLACK){
    rbtree_erase_fixup(t,x);
  }
  free(p);
  return 0;
}

int orderarray(const rbtree *t, node_t *p, key_t *arr, int i, const size_t n){
  if(i < n){
    if(p==t->nil){
      return i;
    }
    i = orderarray(t, p->left, arr, i, n);
    arr[i++] = p->key;
    i = orderarray(t, p->right, arr, i, n);
    return i;
  }
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  orderarray(t, t->root, arr, 0, n);
  return 0;
}
