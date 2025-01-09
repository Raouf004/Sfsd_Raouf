import tkinter as tk
from tkinter import simpledialog, messagebox, filedialog
import json

class FileManager:
    def __init__(self):
        self.file_name = ""
        self.total_records = 0
        self.records = {}

    def create_file(self, name):
        self.file_name = name
        self.total_records = 0
        self.records = {}
        messagebox.showinfo("Info", f"File '{self.file_name}' created.")

    def insert_record(self, record_id, content):
        if record_id in self.records:
            messagebox.showerror("Error", "Record ID already exists.")
        else:
            self.records[record_id] = content
            self.total_records += 1
            messagebox.showinfo("Info", f"Inserted record ID: {record_id}")

    def edit_record(self, record_id, new_content):
        if record_id in self.records:
            self.records[record_id] = new_content
            messagebox.showinfo("Info", f"Updated record ID: {record_id}")
        else:
            messagebox.showerror("Error", "Record ID not found.")

    def delete_record(self, record_id):
        if record_id in self.records:
            del self.records[record_id]
            self.total_records -= 1
            messagebox.showinfo("Info", f"Deleted record ID: {record_id}")
        else:
            messagebox.showerror("Error", "Record ID not found.")

    def display_file(self):
        if not self.records:
            messagebox.showinfo("Info", "No records to display.")
            return
        display_text = f"File: {self.file_name}\n"
        display_text += "\n".join(f"ID: {rid}, Content: {content}" for rid, content in self.records.items())
        messagebox.showinfo("Records", display_text)

    def display_disk_status(self):
        if not self.records:
            messagebox.showinfo("Disk Status", "Disk is empty.")
            return
        display_text = f"File: {self.file_name}\nTotal Records: {self.total_records}"
        messagebox.showinfo("Disk Status", display_text)

    def save_to_file(self):
        if not self.file_name:
            messagebox.showerror("Error", "No file created yet.")
            return
        with open(self.file_name + '.json', 'w') as f:
            json.dump(self.records, f)
            messagebox.showinfo("Info", f"Records saved to {self.file_name}.json")

    def load_from_file(self):
        file_path = filedialog.askopenfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if file_path:
            with open(file_path, 'r') as f:
                self.records = json.load(f)
                self.total_records = len(self.records)
                self.file_name = file_path.split('/')[-1].replace('.json', '')
                messagebox.showinfo("Info", f"Records loaded from {file_path}")

class App:
    def __init__(self, root):
        self.file_manager = FileManager()
        self.root = root
        self.root.title("File Management System")

        # Create buttons
        tk.Button(root, text="Create File", command=self.create_file).pack(pady=10)
        tk.Button(root, text="Insert Record", command=self.insert_record).pack(pady=10)
        tk.Button(root, text="Edit Record", command=self.edit_record).pack(pady=10)
        tk.Button(root, text="Delete Record", command=self.delete_record).pack(pady=10)
        tk.Button(root, text="Display File", command=self.display_file).pack(pady=10)
        tk.Button(root, text="Display Disk Status", command=self.display_disk_status).pack(pady=10)
        tk.Button(root, text="Save to File", command=self.save_to_file).pack(pady=10)
        tk.Button(root, text="Load from File", command=self.load_from_file).pack(pady=10)

    def create_file(self):
        file_name = simpledialog.askstring("Input", "Enter file name:")
        if file_name:
            self.file_manager.create_file(file_name)

    def insert_record(self):
        record_id = simpledialog.askinteger("Input", "Enter record ID:")
        if record_id is not None:
            content = simpledialog.askstring("Input", "Enter record content:")
            if content:
                self.file_manager.insert_record(record_id, content)

    def edit_record(self):
        record_id = simpledialog.askinteger("Input", "Enter record ID to edit:")
        if record_id is not None:
            new_content = simpledialog.askstring("Input", "Enter new record content:")
            if new_content:
                self.file_manager.edit_record(record_id, new_content)

    def delete_record(self):
        record_id = simpledialog.askinteger("Input", "Enter record ID to delete:")
        if record_id is not None:
            self.file_manager.delete_record(record_id)

    def display_file(self):
        self.file_manager.display_file()

    def display_disk_status(self):
        self.file_manager.display_disk_status()

    def save_to_file(self):
        self.file_manager.save_to_file()

    def load_from_file(self):
        self.file_manager.load_from_file()

if __name__ == "__main__":
    root = tk.Tk()
    app = App(root)
    root.mainloop()