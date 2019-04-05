# Generated by Django 2.2 on 2019-04-04 09:47

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='Profile',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('contact_number', models.CharField(max_length=13)),
                ('meter_id', models.CharField(max_length=50)),
                ('user', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL)),
            ],
            options={
                'unique_together': {('meter_id', 'user', 'contact_number')},
            },
        ),
        migrations.CreateModel(
            name='Bill',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('time', models.TimeField(verbose_name='Conversation Time')),
                ('cost', models.FloatField()),
                ('profile', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='rest.Profile')),
            ],
        ),
        migrations.CreateModel(
            name='Record',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('time', models.TimeField(auto_now_add=True)),
                ('volt', models.FloatField()),
                ('current', models.FloatField()),
                ('watt', models.FloatField()),
                ('energy', models.FloatField()),
                ('profile', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='rest.Profile')),
            ],
            options={
                'unique_together': {('profile', 'time')},
            },
        ),
    ]