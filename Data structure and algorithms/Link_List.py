class Node:
    def __init__(self, data):
        self.data = data
        self.next = None

class Single_Link_List:
    def __init__(self):
        self.head = None #Node ตัวแรก
        self.tail = None #Node ตัวสุดท้าย
        self.size = 0 #ขนาดของ Link list

    def __len__(self):
        return self.size

    def add_first(self, item):
        new_node = Node(item)
        if self.head is None:
            self.head = new_node
            self.tail = new_node
        else:
            new_node.next = self.head
            self.head = new_node
        self.size += 1
    def add_last(self, item):
        new_node = Node(item)
        if self.head is None:
            self.head = new_node
            self.tail = new_node
        else:
            self.tail.next = new_node
            self.tail = new_node
        self.size += 1

    def remove_first(self):
        if self.head is not None:
            removed_node = self.head
            self.head = self.head.next
            if self.head is None:
                self.tail = None
            self.size -= 1
            return removed_node
        else:
            return None

     def remove_last(self):
        if self.head is not None:
            if self.head is self.tail:
                removed_node = self.head
                self.head = None
                self.tail = None
                return removed_node

            current_node = self.head
            while current_node.next is not self.tail:
                current_node = current_node.next

            removed_node = self.tail
            current_node.next = None
            self.tail = current_node
            
            self.size -= 1
            return removed_node
        else:
            return None

    def remove_node(self, item):
        previous_node = None
        current_node = self.head
        while current_node is not None and current_node.data != item:
            previous_node = current_node
            current_node = current_node.next

        if current_node is None:
            return None
        else:
            if current_node is self.head:
                self.remove_first()
            elif current_node is self.tail:
                self.remove_last()
            else:
                previous_node.next = current_node.next
                current_node.next = None
                self.size -= 1
                return current_node

    def __repr__(self): #repr(LinkList)
        output = ""
        current_node = self.head
        while current_node is not None:
            output += f"[{current_node.data}]->"
            current_node = current_node.next
        output += "None"
        print(output)
        return output

    def __iter__(self): # list(LinkList)
        current_node = self.head
        while current_node is not None:
            yield current_node.data
            current_node = current_node.next

    def __contains__(self, item):
        current_node = self.head
        while current_node is not None:
            if current_node.data == item:
                return True
            current_node = current_node.next
        return False
